#ifndef _UNEVT_EventPtr_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#ifndef _UNEVT_Event_H_
#  include <UniEvent/Event.h>
#endif
#include <LCore/RefCountPtrT.h>

#define _UNEVT_EventPtr_H_

class UNIEventPtr : public RefCountPtrT<UNIEvent> {
 public:
   typedef RefCountPtrT<UNIEvent> super1;
   static const UNEVT_ClassIdent identifier;

   inline UNIEventPtr(UNIEvent *eptr = 0);
   inline UNIEventPtr(const UNIEventPtr &b);
   inline UNIEventPtr(const super1 &b);

   inline const UNIEventPtr &operator =(const UNIEventPtr &b);
   inline const UNIEventPtr &operator =(const super1 &b);
   inline const UNIEventPtr &operator =(UNIEvent *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline UNIEventPtr::UNIEventPtr(UNIEvent *eptr = 0)
{
   if (eptr) {
      i_SetPtr(eptr);
   }
}

inline UNIEventPtr::UNIEventPtr(const UNIEventPtr &b)
{
   if (b.GetPtr()) {
      i_SetPtr(b.GetPtr());
   }
}

inline UNIEventPtr::UNIEventPtr(const super1 &b)
{
   if (b.GetPtr()) {
      i_SetPtr(b.GetPtr());
   }
}

inline const UNIEventPtr &UNIEventPtr::operator =(const UNIEventPtr &b)
{
   super1::operator =(b);
   return(*this);
}

inline const UNIEventPtr &UNIEventPtr::operator =(const super1 &b)
{
   super1::operator =(b);
   return(*this);
}

inline const UNIEventPtr &UNIEventPtr::operator =(UNIEvent *b)
{
   super1::operator =(b);
   return(*this);
}

#endif
