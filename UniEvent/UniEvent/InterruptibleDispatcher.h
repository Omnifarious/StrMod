#ifndef _UNEVT_InterruptibleDispatcher_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#define _UNEVT_InterruptibleDispatcher_H_

namespace strmod {
namespace unievent {

class EventPtr;

class InterruptibleDispatcher : virtual public Dispatcher {
 public:
   static const UNEVT_ClassIdent identifier;

   //! Because every class (even abstract ones) should have a constructor.
   InterruptibleDispatcher()                        { }
   //! Because abstract classes should have a virtual destructor.
   virtual ~InterruptibleDispatcher()               { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

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

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   //! Purposely left undefined.
   Dispatcher(const Dispatcher &b);
   //! Purposely left undefined.
   const Dispatcher &operator =(const Dispatcher &b);
};

#endif
