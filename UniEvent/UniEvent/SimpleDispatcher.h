#ifndef _UNEVT_SimpleDispatcher_H_  // -*-c++-*-

/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

namespace strmod {
namespace unievent {

class Event;

/** \class SimpleDispatcher SimpleDispatcher.h UniEvent/SimpleDispatcher.h
 * \brief A class that does the minimum necessary to support the Dispatcher
 * interface.
 */
class SimpleDispatcher : public Dispatcher
{
 public:
   //! Create one.
   SimpleDispatcher();
   //! Destroy one.
   virtual ~SimpleDispatcher();

   virtual void addEvent(const EventPtr &ev);

   virtual void dispatchEvents(unsigned int numevents,
			       Dispatcher *enclosing = 0);
   virtual void dispatchUntilEmpty(Dispatcher *enclosing = 0);
   inline virtual void stopDispatching();
   virtual void interrupt();

   virtual bool isQueueEmpty() const;

   virtual void addBusyPollEvent(const EventPtr &ev);
   virtual bool onQueueEmpty(const EventPtr &ev);
   virtual bool onInterrupt(const EventPtr &ev);

 private:
   class Imp;
   Imp &imp_;
   bool stop_flag_;

   inline void i_DispatchEvent(Imp &imp, Dispatcher *enclosing);
   unsigned int i_dispatchNEvents(unsigned int n, bool checkbusypoll,
                                  Dispatcher *enclosing);
   inline unsigned int checkEmptyBusy(Imp &imp, bool &checkbusy);
   void dispatchNEvents(unsigned int n, Dispatcher *enclosing);

   // Purposely left undefined.
   SimpleDispatcher(const SimpleDispatcher &b);
   const SimpleDispatcher &operator =(const SimpleDispatcher &b);
};

//-----------------------------inline functions--------------------------------

void SimpleDispatcher::stopDispatching()
{
   stop_flag_ = true;
}

}; // namespace unievent
}; // namespace strmod

#endif
