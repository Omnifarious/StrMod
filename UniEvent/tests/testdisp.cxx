/* $Header$ */

// See ChangeLog file for log

#define _XOPEN_SOURCE
#include <stdlib.h>
#include "UniEvent/Dispatcher.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include "testdisp.h"
#include <iostream.h>

class CountingEvent : public UNIEvent {
 public:
   inline CountingEvent();
   virtual ~CountingEvent();

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0);

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
}

bool_val TestDispatcher(UNIDispatcher &disp, long seed)
{
   srand48(seed);

   CountingEvent::SetCount(0);
   for (int i = 0; i < 20; i++) {
      disp.AddEvent(new GrowQueueEvent);
   }
   disp.DispatchUntilEmpty();
   cout << " Count == " << CountingEvent::GetCount() << " && "
	<< "Make count == " << CountingEvent::GetMakeCount() << " && "
	<< "Kill count == " << CountingEvent::GetKillCount() << endl;
   return(true);
}
