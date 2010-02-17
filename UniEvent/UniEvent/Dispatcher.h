#ifndef _UNEVT_Dispatcher_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/Protocol.h>
#include <UniEvent/UNEVT_ClassIdent.h>

#define _UNEVT_Dispatcher_H_

class UNIEventPtr;
class OSConditionManager;

//: An interface for a simple queuer and dispatcher of events.
// <p>This provides an interface for classes that want to implement a queue of
// UNIEvent objects that are removed from the queue in FIFO order and 'fired'
// by calling their triggerEvent methods.</p>
class UNIDispatcher : virtual public Protocol {
 public:
   static const UNEVT_ClassIdent identifier;

   UNIDispatcher()                                     { }
   virtual ~UNIDispatcher()                            { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: Add an event to the queue.
   virtual void addEvent(const UNIEventPtr &ev) = 0;

   //: Dispatch a single event.
   inline void dispatchEvent(UNIDispatcher *enclosing = 0);
   //: Dispatch until a certain number of events have been dispatched, or the
   //: queue is empty.
   virtual void dispatchEvents(unsigned int numevents,
			       UNIDispatcher *enclosing = 0) = 0;
   //: Dispatch until the queue is empty.
   virtual void dispatchUntilEmpty(UNIDispatcher *enclosing = 0) = 0;
   //: Cause the multiple event dispatch methods to halt before they normally
   //: would.
   virtual void stopDispatching() = 0;
   //: Does the queue have any events in it?
   virtual bool isQueueEmpty() const = 0;

   //: This event is only triggered whenever an event dispatch is attempted
   //: when there's an empty queue.
   virtual void onQueueEmpty(const UNIEventPtr &ev) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   //: Purposely left undefined.
   UNIDispatcher(const UNIDispatcher &b);
   //: Purposely left undefined.
   const UNIDispatcher &operator =(const UNIDispatcher &b);
};

//-----------------------------inline functions--------------------------------

inline int UNIDispatcher::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline void UNIDispatcher::dispatchEvent(UNIDispatcher *enclosing)
{
   dispatchEvents(1, enclosing);
}

#endif
