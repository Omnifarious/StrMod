#ifndef _UNEVT_Event_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/Protocol.h>
#include <bool.h>
#include <UnEvt/UNEVT_ClassIdent.h>

#define _UNEVT_Event_H_

class UNIDispatcher;

class UNIEvent : virtual public Protocol.h {
 public:
   static const UnEvt_ClassIdent identifier;

   virtual ~UNIEvent()                                 { }

   virtual int AreYouA(const ClassIdent &cid) const;

   // If the event was triggered by a dispatcher, the dispatcher that
   // triggered it is expected to be passed in.  Otherwise NULL (aka 0)
   // can be passed in.
   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   // Purposely left undefined.
   UNIEvent(const UNIEvent &b);
   const UNIEvent &operator =(const UNIEvent &b);
};

#endif
