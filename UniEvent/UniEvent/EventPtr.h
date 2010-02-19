#ifndef _UNEVT_EventPtr_H_  // -*-c++-*-

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

// For log see ../ChangeLog

#ifndef _UNEVT_Event_H_
#  include <UniEvent/Event.h>
#endif
#include <LCore/RefCountPtrT.h>

#define _UNEVT_EventPtr_H_

namespace strmod {
namespace unievent {

class EventPtr : public RefCountPtrT<Event> {
 public:
   typedef RefCountPtrT<Event> super1;
   static const UNEVT_ClassIdent identifier;

   inline EventPtr(Event *eptr = 0);
   inline EventPtr(const EventPtr &b);
   inline EventPtr(const super1 &b);

   inline const EventPtr &operator =(const EventPtr &b);
   inline const EventPtr &operator =(const super1 &b);
   inline const EventPtr &operator =(Event *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline EventPtr::EventPtr(Event *eptr)
{
   if (eptr) {
      i_SetPtr(eptr);
   }
}

inline EventPtr::EventPtr(const EventPtr &b)
{
   if (b.GetPtr()) {
      i_SetPtr(b.GetPtr());
   }
}

inline EventPtr::EventPtr(const super1 &b)
{
   if (b.GetPtr()) {
      i_SetPtr(b.GetPtr());
   }
}

inline const EventPtr &EventPtr::operator =(const EventPtr &b)
{
   super1::operator =(b);
   return(*this);
}

inline const EventPtr &EventPtr::operator =(const super1 &b)
{
   super1::operator =(b);
   return(*this);
}

inline const EventPtr &EventPtr::operator =(Event *b)
{
   super1::operator =(b);
   return(*this);
}

} // namespace unievent
} // namespace strmod

#endif
