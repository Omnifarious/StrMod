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

// For a log, see ./ChangeLog

// $Revision$

#ifdef __GNUG__
#  pragma implementation "UNIXSignalHandler.h"
#endif

#include "UniEvent/UNIXSignalHandler.h"
#include "UniEvent/UNIXError.h"
#include "UniEvent/EventPtr.h"
#include "UniEvent/Dispatcher.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <signal.h>
#include <unistd.h>

namespace strmod {
namespace unievent {
namespace {

const UNEVT_ClassIdent UNIXSignalHandler::identifier(11UL);

UNIXSignalHandler *UNIXSignalHandler::S_curhandler = 0;

typedef std::vector<EventPtr> sigevtlist;
struct sigdata
{
   bool handler_registered_;
   sigevtlist recurring_;
   sigevtlist oneshot_;

   sigdata() : handler_registered_(false) { }
};
typedef std::vector<sigdata> siglist;

class SigInterrupt : public Event
{
 public:
   inline SigInterrupt(UNIXSignalHandler &handler) : parent_(&handler) { }
   inline virtual ~SigInterrupt()  { }

   virtual void triggerEvent(Dispatcher *dispatcher)
   {
      if (parent_)
      {
         parent_->processOutstandingSignals();
      }
   }

   void parentGone()
   {
      parent_ = 0;
   }

 private:
   UNIXSignalHandler *parent_;
};

struct UNIXSignalHandler::Imp
{
   siglist hdlrinfo_;
   sigset_t caught_;
   sigset_t handled_;
   EventPtr siginterrupt_;

   Imp(UNIXSignalHandler &parent) : siginterrupt_(new SigInterrupt(parent))
   {
      sigemptyset(&caught_);
   }
};

class UNIXSignalHandler::SigBlockRegion
{
 public:
   SigBlockRegion(sigset_t &blockedones)
   {
      sigprocmask(SIG_BLOCK, &blockedones, &oldset_);
   }
   ~SigBlockRegion()
   {
      sigprocmask(SIG_SETMASK, &oldset_, 0);
   }

 private:
   sigset_t oldset_;

   SigBlockRegion(const SigBlockRegion &b);
   void operator =(const SigBlockRegion &b);
};

UNIXSignalHandler::UNIXSignalHandler(Dispatcher &dispatcher)
     : disp_(dispatcher), imp_(*(new Imp(*this)))
{
   if (S_curhandler != 0)
   {
      throw std::runtime_error("Tried to instantiate more than one UNIXSignalHandler.");
   }
   S_curhandler = this;
   dispatcher.onInterrupt(imp_.siginterrupt_);
}

UNIXSignalHandler::~UNIXSignalHandler()
{
   if (S_curhandler == this)
   {
      S_curhandler = 0;
   }
   delete &imp_;
}

void UNIXSignalHandler::onSignal(int signo, const EventPtr &ev, bool oneshot)
{
   unsigned int usigno = (signo < 0) ? ~0U : signo;
   if (usigno >= (sizeof(sigset_t) * 8))
   {
      throw std::range_error("signo is too large in UNIXSignalHandler::onSignal");
   }
   else
   {
      if (imp_.hdlrinfo_.size() <= usigno)
      {
         imp_.hdlrinfo_.resize(usigno + 1);
      }
      sigdata &sighdlrinfo = imp_.hdlrinfo_[usigno];
      if (!sighdlrinfo.handler_registered_)
      {
         handleSignal(usigno);
         sighdlrinfo.handler_registered_ = true;
      }
      if (oneshot)
      {
         sighdlrinfo.oneshot_.push_back(ev);
      }
      else
      {
         sighdlrinfo.recurring_.push_back(ev);
      }
   }
}

bool cleanList(unsigned int usigno, sigdata &hdlrinfo,
               const EventPtr &ev, bool oneshot)
{
   sigevtlist &evtlist = oneshot ? hdlrinfo.oneshot_ : hdlrinfo.recurring_;
   sigevtlist &otherlist = oneshot ? hdlrinfo.recurring_ : hdlrinfo.oneshot_;
   if (evtlist.size() > 0)
   {
      std::remove(evtlist.begin(), evtlist.end(), ev);
      if ((evtlist.size() == 0) && (otherlist.size() == 0))
      {
         return true;
      }
   }
   return false;
}

void UNIXSignalHandler::removeEvent(int signo, const EventPtr &ev, bool oneshot)
{
   unsigned int usigno = (signo < 0) ? ~0U : signo;
   if (usigno >= imp_.hdlrinfo_.size())
   {
      return;
   }
   sigdata &thissig = imp_.hdlrinfo_[usigno];
   if (cleanList(usigno, thissig, ev, oneshot))
   {
      unHandleSignal(usigno);
      thissig.handler_registered_ = false;
   }
}

void UNIXSignalHandler::removeEvent(int signo, const EventPtr &ev)
{
   unsigned int usigno = (signo < 0) ? ~0U : signo;
   if (usigno >= imp_.hdlrinfo_.size())
   {
      return;
   }
   sigdata &thissig = imp_.hdlrinfo_[usigno];
   if (cleanList(usigno, thissig, ev, true) ||
       cleanList(usigno, thissig, ev, false))
   {
      unHandleSignal(usigno);
      thissig.handler_registered_ = false;
   }
}

void UNIXSignalHandler::removeEvent(const EventPtr &ev)
{
   const siglist::size_type totalsigs = imp_.hdlrinfo_.size();
   for (unsigned int usigno = 0; usigno < totalsigs; ++usigno)
   {
      sigdata &thissig = imp_.hdlrinfo_[usigno];
      if (cleanList(usigno, thissig, ev, true) ||
          cleanList(usigno, thissig, ev, false))
      {
         unHandleSignal(usigno);
         thissig.handler_registered_ = false;
      }
   }
}

void UNIXSignalHandler::postEventsFor(unsigned int usigno)
{
   sigdata &thissig = imp_.hdlrinfo_[usigno];
   if (thissig.oneshot_.size() > 0)
   {
      const sigevtlist::iterator end = thissig.oneshot_.end();
      sigevtlist::iterator i = thissig.oneshot_.begin();
      for ( ; i != end; ++i)
      {
         disp_.addEvent(*i);
      }
      thissig.oneshot_.clear();
   }
   if (thissig.recurring_.size() > 0)
   {
      const sigevtlist::iterator end = thissig.recurring_.end();
      sigevtlist::iterator i = thissig.recurring_.begin();
      for ( ; i != end; ++i)
      {
         disp_.addEvent(*i);
      }
   }
   else
   {
      unHandleSignal(usigno);
      thissig.handler_registered_ = false;
   }
}

void UNIXSignalHandler::processOutstandingSignals()
{
   sigset_t caughtnow;
   {
      SigBlockRegion blocker(imp_.caught_);
      caughtnow = imp_.caught_;
      sigemptyset(&imp_.caught_);
   }
   {
      const siglist::size_type lastel = imp_.hdlrinfo_.size();
      for (siglist::size_type i = 0; i != lastel; ++i)
      {
         if (sigismember(&caughtnow, i))
         {
            postEventsFor(i);
         }
      }
   }
   disp_.onInterrupt(imp_.siginterrupt_);
}

//  static const char signames[][3] = {
//     "00", "01", "02", "03", "04", "05", "06", "07",
//     "08", "09", "10", "11", "12", "13", "14", "15",
//     "16", "17", "18", "19", "20", "21", "22", "23",
//     "24", "25", "26", "27", "28", "29", "30", "31"
//  };

//  static const char mesg1[] = "Got signal number ";
//  static const char mesg2[] = "!\n";
//  static const char mesg3[] = "Calling sigOccured\n";

void UNIXSignalHandler::signalHandler(int signo)
{
   // These are left here for easy debugging purposes later.
//     ::write(2, mesg1, sizeof(mesg1) - 1);
//     ::write(2, signames[signo], 2);
//     ::write(2, mesg2, sizeof(mesg2) - 1);
   if (S_curhandler)
   {
//        ::write(2, mesg3, sizeof(mesg3) - 1);
      S_curhandler->sigOccured(signo);
   }
}

void UNIXSignalHandler::handleSignal(int signo)
{
   struct ::sigaction act;
   act.sa_handler = signalHandler;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   if (sigaction(signo, &act, 0) != 0)
   {
      throw UNIXError("sigaction", errno, LCoreError(LCORE_GET_COMPILERINFO()));
   }
   sigaddset(&imp_.handled_, signo);
}

void UNIXSignalHandler::unHandleSignal(int signo)
{
   struct ::sigaction act;
   act.sa_handler = SIG_DFL;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   sigaction(signo, &act, 0);
   sigdelset(&imp_.handled_, signo);
}

//  static const char mesg4[] = "In sigOccured\n";

void UNIXSignalHandler::sigOccured(int signo)
{
//     ::write(2, mesg4, sizeof(mesg4) - 1);
   {
      SigBlockRegion blocker(imp_.handled_);
      sigaddset(&imp_.caught_, signo);
   }
   disp_.interrupt();
}

};  // anonymous namespace
};  // namespace unievent
};  // namespace strmod
