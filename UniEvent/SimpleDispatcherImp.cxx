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

#include "UniEvent/SimpleDispatcher.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include <iostream.h>
#include <deque>

typedef deque<UNIEvent *> EVListBase;

class UNISimpleDispatcher::Imp {
 public:
   class EVList : public EVListBase {
    public:
      inline void addElement(UNIEvent *ev);
      inline bool qEmpty();

      inline ~EVList();
   };

   void flopQueues();

   EVList mainq_;
   EVList onempty_;
};

inline void UNISimpleDispatcher::Imp::EVList::addElement(UNIEvent *ev)
{
   ev->AddReference();
   push_back(ev);
}

inline bool UNISimpleDispatcher::Imp::EVList::qEmpty()
{
   return(size() <= 0);
}

inline UNISimpleDispatcher::Imp::EVList::~EVList()
{
   while (!qEmpty())
   {
      UNIEvent *ev = front();

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

void UNISimpleDispatcher::Imp::flopQueues()
{
   while (!onempty_.qEmpty())
   {
      UNIEvent *ev = onempty_.front();
      onempty_.pop_front();
      mainq_.push_back(ev);
   }
}

UNISimpleDispatcher::UNISimpleDispatcher()
     : imp_(*(new Imp)), stop_flag_(false)
{
}

UNISimpleDispatcher::~UNISimpleDispatcher()
{
#ifndef NDEBUG
   if (!imp_.mainq_.qEmpty()) {
      cerr << "Warning!\a  Deleting a UNISimpleDispatcher that isn't empty!\n";
   }
#endif
   delete &imp_;
}

void UNISimpleDispatcher::AddEvent(const UNIEventPtr &ev)
{
   imp_.mainq_.addElement(ev.GetPtr());
}

void UNISimpleDispatcher::DispatchEvents(unsigned int numevents,
					 UNIDispatcher *enclosing)
{
   if (enclosing == 0) {
      enclosing = this;
   }

   unsigned int i = numevents;

   while (i && (!imp_.mainq_.qEmpty()
		|| !imp_.onempty_.qEmpty()) && !stop_flag_)
   {
      if (imp_.mainq_.qEmpty())
      {
	 imp_.flopQueues();
      }
      UNIEvent *ev = imp_.mainq_.front();

      imp_.mainq_.pop_front();
      i--;
      ev->TriggerEvent(enclosing);
      if (ev->NumReferences() > 0)
      {
	    ev->DelReference();
      }
      if (ev->NumReferences() <= 0)
      {
	 delete ev;
      }
   }
   if (stop_flag_)
   {
      stop_flag_ = false;
   }
}

void UNISimpleDispatcher::DispatchUntilEmpty(UNIDispatcher *enclosing)
{
   if (enclosing == 0) {
      enclosing = this;
   }

   while ((!imp_.mainq_.qEmpty() || !imp_.onempty_.qEmpty()) && !stop_flag_)
   {
      if (imp_.mainq_.qEmpty())
      {
	 imp_.flopQueues();
      }

      UNIEvent *ev = imp_.mainq_.front();

      imp_.mainq_.pop_front();
      ev->TriggerEvent(enclosing);
      if (ev->NumReferences() > 0) {
	    ev->DelReference();
      }
      if (ev->NumReferences() <= 0) {
	 delete ev;
      }
   }
   if (stop_flag_) {
      stop_flag_ = false;
   }
}

bool UNISimpleDispatcher::IsQueueEmpty() const
{
   return(imp_.mainq_.qEmpty());
}

void UNISimpleDispatcher::onQueueEmpty(const UNIEventPtr &ev)
{
   imp_.onempty_.addElement(ev.GetPtr());
}
