#ifndef _UNEVT_EventPtrT_H_  // -*-c++-*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
//
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

namespace strmod {
namespace unievent {

template <class Event_t>
class EventPtrT : virtual public EventPtr {
 public:
   typedef EventPtr super1;

   EventPtrT(EventPtrT<Event_t> &b) : super1(b)                              { }
   EventPtrT(RefCountPtrT<Event_t> &b) : super1(b.GetPtr())                  { }
   EventPtrT(Event_t *eptr = 0) : super1(eptr)                               { }

   inline Event_t &operator *() const;
   inline Event_t *operator ->() const;

   inline Event_t *GetPtr() const;

   inline const EventPtrT<Event_t> &operator =(const EventPtrT<Event_t> &b);
   inline const EventPtrT<Event_t> &operator =(const RefCountPtrT<Event_t> &b);
   inline const EventPtrT<Event_t> &operator =(Event_t *b);

 protected:
   inline virtual ReferenceCounting *i_CheckType(ReferenceCounting *p) const;
};

//-----------------------------inline functions--------------------------------

template <class Event_t>
inline Event_t &EventPtrT<Event_t>::operator *() const
{
   return(*GetPtr());
}

template <class Event_t>
inline Event_t *EventPtrT<Event_t>::operator ->() const
{
   return(GetPtr());
}

template <class Event_t>
inline Event_t *EventPtrT<Event_t>::GetPtr() const
{
   return(static_cast<Event_t *>(super1::GetPtr()));
}

template <class Event_t>
inline const EventPtrT<Event_t> &
EventPtrT<Event_t>::operator =(const EventPtrT<Event_t> &b)
{
   super1::operator =(b);
   return(*this);
}

template <class Event_t>
inline const EventPtrT<Event_t> &
EventPtrT<Event_t>::operator =(const RefCountPtrT<Event_t> &b)
{
   super1::operator =(b.GetPtr());
   return(*this);
}

template <class Event_t>
inline const EventPtrT<Event_t> &
EventPtrT<Event_t>::operator =(Event_t *b)
{
   super1::operator =(b);
   return(*this);
}

template <class Event_t>
inline ReferenceCounting *
EventPtrT<Event_t>::i_CheckType(ReferenceCounting *p) const
{
   return(((p != 0) && p->AreYouA(Event::identifier)) ? p : 0);
}

}; // namespace unievent
}; // namespace strmod

#endif
