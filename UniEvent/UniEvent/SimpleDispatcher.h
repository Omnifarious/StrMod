#ifndef _UNEVT_SimpleDispatcher_H_  // -*-c++-*-

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

#include <UniEvent/Dispatcher.h>

#define _UNEVT_SimpleDispatcher_H_

class UNIEvent;

class UNISimpleDispatcher : public UNIDispatcher {
   class Imp;
 public:
   static const UNEVT_ClassIdent identifier;

   UNISimpleDispatcher();
   virtual ~UNISimpleDispatcher();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual void AddEvent(const UNIEventPtr &ev);

   virtual void DispatchEvents(unsigned int numevents,
			       UNIDispatcher *enclosing = 0);
   virtual void DispatchUntilEmpty(UNIDispatcher *enclosing = 0);
   inline virtual void StopDispatching();

   virtual bool IsQueueEmpty() const;

   virtual void onQueueEmpty(const UNIEventPtr &ev);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   Imp &imp_;
   bool stop_flag_;

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
