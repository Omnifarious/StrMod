#ifndef _UNEVT_Event_H_  // -*- mode: c++; c-file-style: "hopper"-*-

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

#include <LCore/Protocol.h>
#include <LCore/RefCounting.h>
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_Event_H_

class UNIDispatcher;
class UNIEventPtr;

/** \class UNIEvent Event.h UniEvent/Event.h
 * \brief An event to be queued up in a UNIDispatcher.
 */
class UNIEvent : virtual public Protocol, public ReferenceCounting {
 public:
   static const UNEVT_ClassIdent identifier;

   //! Nothing exciting here.
   UNIEvent() : ReferenceCounting(0)                   { }
   //! This is an interface class, of course it has a virtual destructor.
   virtual ~UNIEvent()                                 { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   /** Perform the action associated with the event.
    * If the event was triggered by a dispatcher, the dispatcher that triggered
    * it is expected to be passed in.  Otherwise NULL (aka 0) can be passed in.
    * A dispatcher will only call ONE triggerEvent method at a time.
    */
   virtual void triggerEvent(UNIDispatcher *dispatcher = 0) = 0;

   //! Alternate form of TriggerEvent
   inline void operator ()(UNIDispatcher *dispatcher);
   //! Alternate form of TriggerEvent
   inline void operator ()()                           { (*this)(0); }

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

inline void UNIEvent::operator ()(UNIDispatcher *dispatcher)
{
   triggerEvent(dispatcher);
}

#endif
