#ifndef _UNEVT_EventPtrT_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.2  1999/01/10 17:14:14  hopper
// Added a NULL check to i_CheckType.  Not sure if it is right still.
//
// Revision 1.1  1997/05/13 01:03:34  hopper
// Added new EventPtr classes to maintain reference count on UNIEvent classes.
//

#ifndef _UNEVT_EventPtr_H_
#  include <UniEvent/EventPtr.h>
#endif
#ifndef _UNEVT_Event_H_
#  include <UniEvent/Event.h>
#endif

#define _UNEVT_EventPtrT_H_

template <class Event>
class UNIEventPtrT : virtual public UNIEventPtr {
 public:
   typedef UNIEventPtr super1;

   UNIEventPtrT(UNIEventPtrT<Event> &b) : super1(b)                         { }
   UNIEventPtrT(RefCountPtrT<Event> &b) : super1(b.GetPtr())                { }
   UNIEventPtrT(Event *eptr = 0) : super1(eptr)                             { }

   inline Event &operator *() const;
   inline Event *operator ->() const;

   inline Event *GetPtr() const;

   inline const UNIEventPtrT<Event> &operator =(const UNIEventPtrT<Event> &b);
   inline const UNIEventPtrT<Event> &operator =(const RefCountPtrT<Event> &b);
   inline const UNIEventPtrT<Event> &operator =(Event *b);

 protected:
   inline virtual RC *i_CheckType(RC *p) const;
};

//-----------------------------inline functions--------------------------------

template <class Event>
inline Event &UNIEventPtrT<Event>::operator *() const
{
   return(*GetPtr());
}

template <class Event>
inline Event *UNIEventPtrT<Event>::operator ->() const
{
   return(GetPtr());
}

template <class Event>
inline Event *UNIEventPtrT<Event>::GetPtr() const
{
   return(static_cast<Event *>(super1::GetPtr()));
}

template <class Event>
inline const UNIEventPtrT<Event> &
UNIEventPtrT<Event>::operator =(const UNIEventPtrT<Event> &b)
{
   super1::operator =(b);
   return(*this);
}

template <class Event>
inline const UNIEventPtrT<Event> &
UNIEventPtrT<Event>::operator =(const RefCountPtrT<Event> &b)
{
   super1::operator =(b.GetPtr());
   return(*this);
}

template <class Event>
inline const UNIEventPtrT<Event> &
UNIEventPtrT<Event>::operator =(Event *b)
{
   super1::operator =(b);
   return(*this);
}

template <class Event>
inline ReferenceCounting *
UNIEventPtrT<Event>::i_CheckType(ReferenceCounting *p) const
{
   return(((p != 0) && p->AreYouA(Event::identifier)) ? p : 0);
}

#endif
