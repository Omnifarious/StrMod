#ifndef _UNEVT_Dispatcher_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/Protocol.h>
#include <bool.h>
#include <UniEvent/UNEVT_ClassIdent.h>

#define _UNEVT_Dispatcher_H_

class UNIEventPtr;
class OSConditionManager;

class UNIDispatcher : virtual public Protocol {
 public:
   static const UNEVT_ClassIdent identifier;

   UNIDispatcher()                                     { }
   virtual ~UNIDispatcher()                            { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual void AddEvent(const UNIEventPtr &ev) = 0;

   inline void DispatchEvent(UNIDispatcher *enclosing = 0);
   virtual void DispatchEvents(unsigned int numevents,
			       UNIDispatcher *enclosing = 0) = 0;
   virtual void DispatchUntilEmpty(UNIDispatcher *enclosing = 0) = 0;
   virtual void StopDispatching() = 0;
   virtual bool_val IsQueueEmpty() const = 0;

   // Used to enforce a 1->0,1 relationship between dispatchers and
   // condition managers.  Having more than one condition manager per
   // dispatcher is a bad thing since they'll step on eachother's toes.
   virtual OSConditionManager *GetConditionManager()   { return(0); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   // Purposely left undefined.
   UNIDispatcher(const UNIDispatcher &b);
   const UNIDispatcher &operator =(const UNIDispatcher &b);
};

//-----------------------------inline functions--------------------------------

inline int UNIDispatcher::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline void UNIDispatcher::DispatchEvent(UNIDispatcher *enclosing)
{
   DispatchEvents(1, enclosing);
}

#endif
