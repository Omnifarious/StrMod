#ifndef _UNEVT_SimpleDispatcher_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <UniEvent/Dispatcher.h>
#include <bool.h>

#define _UNEVT_SimpleDispatcher_H_

class UNIEvent;

class UNISimpleDispatcher : public UNIDispatcher {
   class Imp;
 public:
   static const UNEVT_ClassIdent identifier;

   UNISimpleDispatcher();
   virtual ~UNISimpleDispatcher();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual void AddEvent(UNIEvent *ev);

   virtual void DispatchEvents(unsigned int numevents);
   virtual void DispatchUntilEmpty();
   inline virtual void StopDispatching();

   virtual bool_val IsQueueEmpty() const;

   virtual OSConditionManager *GetConditionManager()   { return(0); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   Imp &imp_;
   bool_val stop_flag_;

   // Purposely left undefined.
   UNISimpleDispatcher(const UNISimpleDispatcher &b);
   const UNISimpleDispatcher &operator =(const UNISimpleDispatcher &b);
};

//-----------------------------inline functions--------------------------------

inline int UNISimpleDispatcher::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || UNIDispatcher::AreYouA(cid));
}

void UNISimpleDispatcher::StopDispatching()
{
   stop_flag_ = true;
}

#endif
