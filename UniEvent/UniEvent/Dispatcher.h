#ifndef _UNEVT_Dispatcher_H_  // -*-c++-*-

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

#ifndef _UNEVT_EventPtr_H_
#  include <UniEvent/EventPtr.h>
#endif

#define _UNEVT_Dispatcher_H_

namespace strmod {
namespace unievent {

/** \class Dispatcher Dispatcher.h UniEvent/Dispatcher.h
 * \brief An interface for a simple queuer and dispatcher of events.
 *
 * This provides an interface for classes that want to implement a queue of
 * Event objects that are removed from the queue in FIFO order and 'fired' by
 * calling their triggerEvent methods.
 */
class Dispatcher
{
 public:
   //! Because every class (even abstract ones) should have a constructor.
   Dispatcher()                                     { }
   //! Because abstract classes should have a virtual destructor.
   virtual ~Dispatcher() = default;

   //! Add an event to the queue.
   virtual void addEvent(const EventPtr &ev) = 0;

   /**
    * Add an event that will be posted to poll something if the queue is busy.
    *
    * The Dispatcher maintains the concept of <em>external</em>
    * and <em>internal</em> events.  External events are those posted by the
    * addEvent function.  Internal events are those posted in response to some
    * internal condition.
    *
    * When external events cause other external events to be added, busy loops
    * can take over the system.  In order to prevent this, an event from this
    * queue will be posted if the queue contains only external events.
    *
    * Events in this queue should query some outside event source for events and
    * use addEvent to add any there may be.  <strong>Under no circumstance
    * should one of these events <em>always</em> cause events to be added
    * through addEvent.</strong> Doing this would just propogate the bad busy
    * loop behavior.
    */
   virtual void addBusyPollEvent(const EventPtr &ev) = 0;

   /**
    * \brief This event is only triggered whenever an event dispatch is
    * attempted when there's an empty queue.
    *
    * @param ev The event to fire when the queue is empty.
    * @return <code>true</code> if the event was successfully set,
    * <code>false</code> if the event was not set because some other event was
    * already set.
    */
   virtual bool onQueueEmpty(const EventPtr &ev) = 0;

   /**
    * \brief This event is only triggered when interrupt() is called.
    *
    * When interrupt() is called, a flag is set so this event is put onto the
    * front of the queue as soon as the Dispatcher's event loop gets control
    * again.
    *
    * @param ev The event to fire when interrupt() is called.
    * @return <code>true</code> if the event was successfully set,
    * <code>false</code> if the event was not set because some other event was
    * already set.
    */
   virtual bool onInterrupt(const EventPtr &ev) = 0;

   //! Dispatch a single event.
   inline void dispatchEvent(Dispatcher *enclosing = 0);

   /**
    * \brief Dispatch until a certain number of events have been dispatched, or
    * the queue is empty.
   */
   virtual void dispatchEvents(unsigned int numevents,
			       Dispatcher *enclosing = 0) = 0;
   //! Dispatch until the queue is empty.
   virtual void dispatchUntilEmpty(Dispatcher *enclosing = 0) = 0;

   /**
    * \brief Interrupts the dispatcher to execute a high priority event.
    *
    * This sets a flag so that as soon as the Dispatcher's event loop gets
    * control, the event set by onInterrupt() will be put on the front of the
    * queue.  It will also call the Event::interrupt() method of any
    * currently executing event.
    *
    * \note Implementations of this method should be safe to call from a thread
    * or a signal handling context.
    */
   virtual void interrupt() = 0;

   /**
    * Cause the multiple event dispatch methods to halt before they normally
    * would.
    */
   virtual void stopDispatching() = 0;
   //! Does the queue have any events in it?
   virtual bool isQueueEmpty() const = 0;

 private:
   //! Purposely left undefined.
   Dispatcher(const Dispatcher &b);
   //! Purposely left undefined.
   const Dispatcher &operator =(const Dispatcher &b);
};

//-----------------------------inline functions--------------------------------

inline void Dispatcher::dispatchEvent(Dispatcher *enclosing)
{
   dispatchEvents(1, enclosing);
}

} // namespace unievent
} // namespace strmod

#endif
