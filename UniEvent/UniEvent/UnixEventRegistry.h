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
class UnixEventRegistry
{
 public:
   /** These enums are intended to be used with the enum_set, FDCondSet.
    * In maintaining this enum, FD_Readable must be first, and FD_Invalid must
    * be last.
    * \note fd means file descriptor.
    */
   enum FDConditions {
      FD_Readable, //!< Can the fd be read from without blocking?
      FD_Writeable, //!< Can the fd be written to without blocking?
      FD_Error,  //!< Is the fd in some kind of error state?
      FD_Closed, //!< Has the fd been closed by another process?
      FD_Invalid //!< Was the fd never opened?
   };
   //! A set of 0 or more FDConditions.
   typedef enum_set<FDConditions, FD_Readable, FD_Invalid> FDCondSet;
   //! The set of all FDConditions
   static const FDCondSet all_fdconds;

   //! This is an abstract base class, so the constructor has little meaning.
   inline UnixEventRegistry();
   //! An abstract base class with no members, has nothing to destruct.
   inline virtual ~UnixEventRegistry();

   //! Register the event '*ev' to be fired on file descriptor condition true.
   virtual void registerFDCond(int fd,
                               const FDCondSet &condbits,
                               const EventPtr &ev) = 0;

   //! Removes all entries associated with a particular file descriptor.
   virtual void freeFD(int fd) = 0;

   /** On the given signal, post the given event.
    * @param signo The number of the signal to post an event for.
    * @param ev The event to post.
    */
   virtual void onSignal(int signo, const EventPtr &e) = 0;

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
    * when the signal happens.
    */
   virtual void clearSignal(int signo) = 0;

   //! Actually call the UNIX poll system call, and dispatch resulting events.
   virtual void doPoll(bool wait = false) = 0;

 protected:
   //! Maximum number of signals the handled_by_S can keep track of
   static const unsigned int max_handled_by_S = 128;
   /** Keeps track of which UnixEventRegistry is handling which signal.
    *
    * This is so that instances of derived classes (since this is an abstract
    * class) have a scratchpad to use to communicate with eachother about who's
    * handling which signal.  This scratchpad is used both to allow actual
    * signal handlers to know which class to poke when a signal does arrive, and
    * also so that different handlers don't both try to handle the same signal.
    */
   static UnixEventRegistry *handled_by_S[max_handled_by_S];
};

//-----------------------------inline functions--------------------------------

inline UnixEventRegistry::UnixEventRegistry()
{
}

inline UnixEventRegistry::~UnixEventRegistry()
{
}

} // namespace unievent
} // namespace strmod

#endif
