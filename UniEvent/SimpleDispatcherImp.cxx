/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Header$ */

// Log can be found in ChangeLog

#ifdef __GNUG__
#  pragma implementation "SimpleDispatcher.h"
#endif

#include "UniEvent/SimpleDispatcher.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include <iostream>
#include <deque>
#include <signal.h>
//  #include <unistd.h>

namespace strmod {
namespace unievent {

//! Private implementation details of SimpleDispatcher.
class SimpleDispatcher::Imp {
 public:
   typedef std::deque<Event *> EVListBase;
   class EVList : private EVListBase {
    public:
      inline void addElement(Event *ev);
      inline bool qEmpty();
      inline size_t size() const { return EVListBase::size(); }
      inline void moveFrontTo(EVList &b);
      inline Event *noref_pop_front();

      inline ~EVList();
   };

   EVList mainq_;
   EVList busypoll_;
   size_t internalevnum_;
   EventPtr onempty_;
   EventPtr oninterrupt_;
   volatile sig_atomic_t interrupted_;
   Event *curevt_;
};

namespace {

void *interruptBlock();
void interruptUnblock(void *data);

}

inline void SimpleDispatcher::Imp::EVList::addElement(Event *ev)
{
   ev->AddReference();
   push_back(ev);
}

inline bool SimpleDispatcher::Imp::EVList::qEmpty()
{
   return(size() <= 0);
}

inline SimpleDispatcher::Imp::EVList::~EVList()
{
   while (!qEmpty())
   {
      Event *ev = front();

      pop_front();
      if (ev->NumReferences() > 0)
      {
	 ev->DelReference();
      }
      if (ev->NumReferences() <= 0)
      {
	 delete ev;
      }
   }
}

inline void SimpleDispatcher::Imp::EVList::moveFrontTo(EVList &b)
{
   assert(size() > 0);
   Event *ev = front();
   pop_front();
   b.push_back(ev);
}

//! Remove and return the front element without altering the reference count.
inline Event *SimpleDispatcher::Imp::EVList::noref_pop_front()
{
   assert(size() > 0);
   Event *ev = front();
   pop_front();
   return ev;
}

SimpleDispatcher::SimpleDispatcher()
     : imp_(*(new Imp)), stop_flag_(false)
{
   imp_.internalevnum_ = 0;
   imp_.interrupted_ = 0;
   imp_.curevt_ = 0;
}

SimpleDispatcher::~SimpleDispatcher()
{
#ifndef NDEBUG
   if (!imp_.mainq_.qEmpty()) {
      std::cerr << "Warning!\a  Deleting a SimpleDispatcher that isn't empty!\n";
   }
#endif
   delete &imp_;
}

void SimpleDispatcher::addEvent(const EventPtr &ev)
{
   imp_.mainq_.addElement(ev.GetPtr());
}

void SimpleDispatcher::addBusyPollEvent(const EventPtr &ev)
{
   imp_.busypoll_.addElement(ev.GetPtr());
}

inline void SimpleDispatcher::i_DispatchEvent(Imp &imp,
                                              Dispatcher *enclosing)
{
   assert(!imp.mainq_.qEmpty());
   assert(enclosing != 0);

   Event *ev = 0;

   {
      void *tmp = interruptBlock();
      if (imp_.interrupted_)
      {
         imp_.interrupted_ = 0;
         ev = imp_.oninterrupt_.GetPtr();
         if (ev)
         {
            ev->AddReference();
            imp_.oninterrupt_.ReleasePtr();
         }
      }
      if (!ev)
      {
         ev = imp.mainq_.noref_pop_front();
      }
      imp_.curevt_ = ev;
      interruptUnblock(tmp);
   }
   ev->triggerEvent(enclosing);
   {
      void *tmp = interruptBlock();
      imp_.curevt_ = 0;
      interruptUnblock(tmp);
   }
   if (ev->NumReferences() > 0)
   {
      ev->DelReference();
   }
   if (ev->NumReferences() <= 0)
   {
      delete ev;
   }
}

unsigned int SimpleDispatcher::i_dispatchNEvents(unsigned int n,
                                                 bool checkbusypoll,
                                                 Dispatcher *enclosing)
{
   assert(imp_.mainq_.size() >= n);
   assert(enclosing != 0);

   Imp &imp = imp_;
   unsigned int i = n;
   for (; (i > 0) && !stop_flag_; --i)
   {
      assert(!imp.mainq_.qEmpty() && imp.internalevnum_ > 0);
      i_DispatchEvent(imp, enclosing);
      if (checkbusypoll && ! imp.busypoll_.qEmpty())
      {
         break;
      }
   }
   return n - i;
}

inline unsigned int
SimpleDispatcher::checkEmptyBusy(Imp &imp, bool &checkbusy)
{
   assert(imp.internalevnum_ <= imp.mainq_.size());
   unsigned int retval = imp.internalevnum_;
   checkbusy = false;
   if (imp.internalevnum_ == 0)
   {
      if (imp.mainq_.qEmpty())
      {
         if (imp.onempty_)  // mainq empty, and there's an 'onempty' event.
         {
            // Post 'onempty' event to mainq.
            imp.mainq_.addElement(imp.onempty_.GetPtr());
            imp.internalevnum_ = 1;
            imp.onempty_.ReleasePtr();
            retval = 1;
         }
      }
      else  // mainq_ isn't empty, so must be full of external events.
      {
         if (! imp.busypoll_.qEmpty())  // There are busypoll events to post.
         {
            // Post an event from the buspoll list and record where the
            // internal event is on mainq.
            imp.busypoll_.moveFrontTo(imp.mainq_);
            imp.internalevnum_ = imp.mainq_.size();
         }
         else
         {
            // Pretend this event is an internal event so we check again next
            // time through loop and there isn't any special case handling.
            imp.internalevnum_ = 1;
            checkbusy = true;
         }
      }
      retval = imp.mainq_.size();
   }
   return retval;
}

void SimpleDispatcher::dispatchNEvents(unsigned int numevents,
                                       Dispatcher *enclosing)
{
   assert(numevents > 0);
   assert(enclosing != 0);

   Imp &imp = imp_;

   while (numevents > 0 && !stop_flag_)
   {
      bool checkbusy = false;
      unsigned int evchunk = checkEmptyBusy(imp, checkbusy);
      if (evchunk <= 0)
      {
         break;
      }
      else
      {
         if (evchunk > numevents)
         {
            evchunk = numevents;
         }
         unsigned int dispatched = i_dispatchNEvents(evchunk, checkbusy,
                                                     enclosing);
         if (dispatched > imp.internalevnum_)
         {
            imp.internalevnum_ = 0;
         }
         else
         {
            imp.internalevnum_ -= dispatched;
         }
         numevents -= dispatched;
      }
   }
}

void SimpleDispatcher::dispatchEvents(unsigned int numevents,
                                      Dispatcher *enclosing)
{
   if (numevents <= 0)
   {
      return;
   }
   else
   {
      if (enclosing == 0)
      {
         enclosing = this;
      }
      dispatchNEvents(numevents, enclosing);
   }
   if (stop_flag_)
   {
      stop_flag_ = false;
   }
}

void SimpleDispatcher::dispatchUntilEmpty(Dispatcher *enclosing)
{
   if (enclosing == 0)
   {
      enclosing = this;
   }
   while (!stop_flag_ && ((imp_.mainq_.size() > 0) || imp_.onempty_))
   {
      dispatchNEvents(imp_.mainq_.size() * 2 + 1, enclosing);
   }
   if (stop_flag_)
   {
      stop_flag_ = false;
   }
}

bool SimpleDispatcher::isQueueEmpty() const
{
   return(imp_.mainq_.qEmpty());
}

bool SimpleDispatcher::onQueueEmpty(const EventPtr &ev)
{
   if (imp_.onempty_)
   {
      return false;
   }
   else
   {
      imp_.onempty_ = ev;
      return true;
   }
}

bool SimpleDispatcher::onInterrupt(const EventPtr &ev)
{
   if (imp_.oninterrupt_)
   {
      return false;
   }
   else
   {
      imp_.oninterrupt_ = ev;
      return true;
   }
}

//  static const char mesg1[] = "In interrupt\n";
//  static const char mesg2[] = "Not interrupted\n";

void SimpleDispatcher::interrupt()
{
//     ::write(2, mesg1, sizeof(mesg1) - 1);
   if (!imp_.interrupted_)
   {
//        ::write(2, mesg2, sizeof(mesg2) - 1);
      void *tmp = interruptBlock();
      if (!imp_.interrupted_)
      {
         imp_.interrupted_ = 1;
         if (imp_.curevt_)
         {
            imp_.curevt_->interrupt();
         }
      }
      interruptUnblock(tmp);
   }
}

namespace {
void *interruptBlock()
{
   sigset_t *oldset = new sigset_t;
   sigset_t newset;
   sigfillset(&newset);
   ::sigprocmask(SIG_BLOCK, &newset, oldset);
   return oldset;
}

void interruptUnblock(void *data)
{
   sigset_t *origset = static_cast<sigset_t *>(data);
   ::sigprocmask(SIG_SETMASK, origset, NULL);
   delete origset;
}
}

} // namespace unievent
} // namespace strmod
