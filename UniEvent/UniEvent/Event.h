#ifndef _UNEVT_Event_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/Protocol.h>
#include <LCore/RefCounting.h>
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_Event_H_

class UNIDispatcher;
class UNIEventPtr;

class UNIEvent : virtual public Protocol, public ReferenceCounting {
 public:
   static const UNEVT_ClassIdent identifier;

   UNIEvent() : ReferenceCounting(0)                   { }
   virtual ~UNIEvent()                                 { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   // If the event was triggered by a dispatcher, the dispatcher that
   // triggered it is expected to be passed in.  Otherwise NULL (aka 0)
   // can be passed in.
   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0) = 0;

   // Alternate form...
   inline void operator ()(UNIDispatcher *dispatcher = 0);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   // Purposely left undefined.
   UNIEvent(const UNIEvent &b);
   const UNIEvent &operator =(const UNIEvent &b);
};

//-----------------------------inline functions--------------------------------

inline int UNIEvent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline void UNIEvent::operator ()(UNIDispatcher *dispatcher = 0)
{
   TriggerEvent(dispatcher);
}

#endif
