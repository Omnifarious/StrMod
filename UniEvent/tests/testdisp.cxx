/* $Header$ */

// See ChangeLog file for log

#include "UniEvent/Dispatcher.h"
#include "UniEvent/Event.h"
#include "testdisp.h"
#include <stdlib.h>
#include <iostream.h>

class CountingEvent : public UNIEvent {
 public:
   CountingEvent()                                  { this_count_ = count_++; }
   virtual ~CountingEvent()                         { }

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0);

   static unsigned int GetCount()                   { return(count_); }
   static void SetCount(unsigned int ct)            { count_ = ct; }

 protected:
   unsigned int MyCount() const                     { return(this_count_); }

 private:
   static unsigned int count_;
   unsigned int this_count_;
};

unsigned int CountingEvent::count_ = 0;

void CountingEvent::TriggerEvent(UNIDispatcher *dispatcher)
{
   cout << "Event with count " << this_count_ << " triggered!\n";
}


class GrowQueueEvent : public CountingEvent {
 public:
   GrowQueueEvent()                                                         { }
   inline virtual ~GrowQueueEvent()                                         { }

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0);
};

class ShrinkQueueEvent : public CountingEvent {
 public:
   ShrinkQueueEvent()                                                       { }
   virtual ~ShrinkQueueEvent()                                              { }

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0);
};

void GrowQueueEvent::TriggerEvent(UNIDispatcher *dispatcher)
{
   CountingEvent::TriggerEvent(dispatcher);

   if (dispatcher) {
      int chance = lrand48() % 4;

      if ((chance == 0) || (chance == 1)) {
	 if (MyCount() > 1024) {
	    dispatcher->AddEvent(new ShrinkQueueEvent);
	    dispatcher->AddEvent(new ShrinkQueueEvent);
	 } else {
	    dispatcher->AddEvent(new GrowQueueEvent);
	    dispatcher->AddEvent(new GrowQueueEvent);
	 }
      } else if (chance == 2) {
	 if (MyCount() > 1024) {
	    dispatcher->AddEvent(new ShrinkQueueEvent);
	 } else {
	    dispatcher->AddEvent(new GrowQueueEvent);
	 }
      }
   }
   delete this;
}

void ShrinkQueueEvent::TriggerEvent(UNIDispatcher *dispatcher)
{
   CountingEvent::TriggerEvent(dispatcher);

   if (dispatcher) {
      int chance = lrand48() % 4;

      if (chance == 0) {
	 dispatcher->AddEvent(new ShrinkQueueEvent);
	 dispatcher->AddEvent(new ShrinkQueueEvent);
      } else if (chance == 1) {
	 dispatcher->AddEvent(new ShrinkQueueEvent);
      }
   }
   delete this;
}

bool_val TestDispatcher(UNIDispatcher &disp, long seed)
{
   srand48(seed);

   CountingEvent::SetCount(0);
   for (int i = 0; i < 20; i++) {
      disp.AddEvent(new GrowQueueEvent);
   }
   disp.DispatchUntilEmpty();
   return(true);
}
