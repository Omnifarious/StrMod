/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// See ../ChangeLog file for log

#define _XOPEN_SOURCE
#include <cstdlib>
#include "UniEvent/Dispatcher.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include "testdisp.h"
#include <iostream>

class CountingEvent : public UNIEvent {
 public:
   inline CountingEvent();
   virtual ~CountingEvent();

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0);

   static unsigned int GetCount()                      { return(count_); }
   static void SetCount(unsigned int ct)               { count_ = ct; }

   static unsigned int GetMakeCount()                  { return(make_count_); }
   static unsigned int GetKillCount()                  { return(kill_count_); }

 protected:
   unsigned int MyCount() const                        { return(this_count_); }

 private:
   static unsigned int count_;
   static unsigned int make_count_;
   static unsigned int kill_count_;
   unsigned int this_count_;
};

unsigned int CountingEvent::count_ = 0;
unsigned int CountingEvent::make_count_ = 0;
unsigned int CountingEvent::kill_count_ = 0;

inline CountingEvent::CountingEvent()
{
   this_count_ = count_++;
   make_count_++;
}

CountingEvent::~CountingEvent()
{
   kill_count_++;
}

void CountingEvent::triggerEvent(UNIDispatcher *dispatcher)
{
   cout << "Event with count " << this_count_ << " triggered!\n";
}


class GrowQueueEvent : public CountingEvent {
 public:
   GrowQueueEvent() = default;
   inline virtual ~GrowQueueEvent() = default;

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0);
};

class ShrinkQueueEvent : public CountingEvent {
 public:
   ShrinkQueueEvent() = default;
   virtual ~ShrinkQueueEvent() = default;

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0);
};

void GrowQueueEvent::triggerEvent(UNIDispatcher *dispatcher)
{
   CountingEvent::triggerEvent(dispatcher);

   if (dispatcher) {
      int chance = lrand48() % 4;

      if ((chance == 0) || (chance == 1)) {
	 if (MyCount() > 1024) {
	    dispatcher->addEvent(new ShrinkQueueEvent);
	    dispatcher->addEvent(new ShrinkQueueEvent);
	 } else {
	    dispatcher->addEvent(new GrowQueueEvent);
	    dispatcher->addEvent(new GrowQueueEvent);
	 }
      } else if (chance == 2) {
	 if (MyCount() > 1024) {
	    dispatcher->addEvent(new ShrinkQueueEvent);
	 } else {
	    dispatcher->addEvent(new GrowQueueEvent);
	 }
      }
   }
}

void ShrinkQueueEvent::triggerEvent(UNIDispatcher *dispatcher)
{
   CountingEvent::triggerEvent(dispatcher);

   if (dispatcher) {
      int chance = lrand48() % 4;

      if (chance == 0) {
	 dispatcher->addEvent(new ShrinkQueueEvent);
	 dispatcher->addEvent(new ShrinkQueueEvent);
      } else if (chance == 1) {
	 dispatcher->addEvent(new ShrinkQueueEvent);
      }
   }
}

bool TestDispatcher(UNIDispatcher &disp, long seed)
{
   srand48(seed);

   CountingEvent::SetCount(0);
   for (int i = 0; i < 20; i++) {
      disp.addEvent(new GrowQueueEvent);
   }
   disp.dispatchUntilEmpty();
   cout << " Count == " << CountingEvent::GetCount() << " && "
	<< "Make count == " << CountingEvent::GetMakeCount() << " && "
	<< "Kill count == " << CountingEvent::GetKillCount() << endl;
   return(true);
}
