/* $Header$ */

// Log can be found in ChangeLog

#include "UniEvent/SimpleDispatcher.h"
#include "UniEvent/Event.h"
#include <deque>

typedef deque<UNIEvent *> ImpBase;

class UNISimpleDispatcher::Imp : public ImpBase {
};

static inline bool_val QEmpty(const ImpBase &imp)
{
   return(imp.size() == 0);
}

UNISimpleDispatcher::UNISimpleDispatcher()
     : imp_(*(new Imp)), stop_flag_(false)
{
}

UNISimpleDispatcher::~UNISimpleDispatcher()
{
#ifndef NDEBUG
   if (!QEmpty(imp_)) {
      cerr << "Warning!\a  Deleting a UNISimpleDispatcher that isn't empty!\n";
   }
#endif
   delete (&imp_);
}

void UNISimpleDispatcher::AddEvent(UNIEvent *ev)
{
   imp_.push_back(ev);
}

void UNISimpleDispatcher::DispatchEvents(unsigned int numevents,
					 UNIDispatcher *enclosing)
{
   if (enclosing == 0) {
      enclosing = this;
   }

   unsigned int i = numevents;

   while (i && !QEmpty(imp_) && !stop_flag_) {
      UNIEvent *ev = imp_.front();

      imp_.pop_front();
      i--;
      ev->TriggerEvent(enclosing);
   }
   if (stop_flag_) {
      stop_flag_ = false;
   }
}

void UNISimpleDispatcher::DispatchUntilEmpty(UNIDispatcher *enclosing)
{
   if (enclosing == 0) {
      enclosing = this;
   }

   while (!QEmpty(imp_) && !stop_flag_) {
      UNIEvent *ev = imp_.front();

      imp_.pop_front();
      ev->TriggerEvent(enclosing);
   }
   if (stop_flag_) {
      stop_flag_ = false;
   }
}

bool_val UNISimpleDispatcher::IsQueueEmpty() const
{
   return(QEmpty(imp_));
}
