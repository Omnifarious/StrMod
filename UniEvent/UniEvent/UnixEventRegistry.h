#ifndef _UNEVT_UnixEventRegistry_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <LCore/enum_set.h>

#define _UNEVT_UnixEventRegistry_H_

namespace strmod {
namespace unievent {

class Dispatcher;
class EventPtr;

/** \class UnixEventRegistry UnixEventRegistry.h UniEvent/UnixEventRegistry.h
 * \brief Manages events associated with various file descriptors and/or
 * signals.
 */
class UnixEventRegistry {
 public:
   //! These enums are intended to be used with the enum_set, FDCondSet.
   enum FDConditions { FD_Readable, FD_Writeable,
                       FD_Error, FD_Closed, FD_Invalid };
   //! A set of 0 or more FDConditions.
   typedef enum_set<FDConditions, FD_Readable, FD_Invalid> FDCondSet;
   static const FDCondSet all_fdconds;

   inline UnixEventRegistry(Dispatcher *dispatcher);
   inline virtual ~UnixEventRegistry();

   //! Register the event '*ev' to be fired on file descriptor condition true.
   virtual bool registerFDCond(int fd,
                               const FDCondSet &condbits,
                               const EventPtr &ev) = 0;

   //! Removes all entries associated with a particular file descriptor.
   virtual void freeFD(int fd) = 0;

   /** On the given signal, post the given event.
    * @param signo The number of the signal to post an event for.
    * @param ev The event to post.
    * @param oneshot Whether or not the event is removed after it is
    * posted (defualts to true).
    */
   virtual void onSignal(int signo, const EventPtr &e, bool oneshot = true) = 0;

   /** Stop posting the given event for the given signal.
    * @param signo The signal to stop posting the event for.
    * @param ev The event to stop posting.
    */
   virtual void clearSignal(int signo, const EventPtr &e) = 0;

   /** Stop posting any events for the given signal.
    * @param signo The signal to stop posting events for.
    *
    * This function should be avoided as you may interfere unkowingly with other
    * parts of the program that are still depending on their events being posted
    * when the signal happening.
    */
   virtual void clearSignal(int signo) = 0;

   //! Actually call the UNIX poll system call, and dispatch resulting events.
   virtual void doPoll(bool wait = false) = 0;

 protected:
   Dispatcher *getDispatcher() const                { return(disp_); }

 private:
   Dispatcher * const disp_;
};

//-----------------------------inline functions--------------------------------

inline UnixEventRegistry::UnixEventRegistry(Dispatcher *dispatcher)
   : disp_(dispatcher)
{
}

inline UnixEventRegistry::~UnixEventRegistry()
{
}

} // namespace unievent
} // namespace strmod

#endif
