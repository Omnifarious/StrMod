#ifndef _UNEVT_Dispatcher_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/Protocol.h>
#include <bool.h>
#include <UnEvt/UNEVT_ClassIdent.h>

#define _UNEVT_Dispatcher_H_

class UNIEvent;
class OSConditionManager;

class UNIDispatcher : virtual public Protocol {
 public:
   static const UnEvt_ClassIdent identifier;

   virtual ~UNIDispatcher()                            { }

   virtual int AreYouA(const ClassIdent &cid) const;

   virtual void AddEvent(UNIEvent *ev) = 0;

   void DispatchEvent()                                { DispatchEvents(1); }
   virtual void DispatchEvents(unsigned int numevents) = 0;
   virtual void DispatchUntilEmpty() = 0;
   virtual void StopDispatching() = 0;
   virtual bool_ret IsQueueEmpty() const = 0;

   virtual OSConditionManager *GetConditionManager()   { return(0); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   // Purposely left undefined.
   UNIDispatcher(const UNIDispatcher &b);
   const UNIDispatcher &operator =(const UNIDispatcher &b);
};

//-----------------------------inline functions--------------------------------

virtual int AreYouA(const ClassIdent &cid) const;
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
