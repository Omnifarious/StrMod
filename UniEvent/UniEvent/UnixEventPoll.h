#ifndef _UNEVT_UnixEventPoll_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#ifndef _UNEVT_UnixEventRegistry_H_
#  include <UniEvent/UnixEventRegistry.h>
#endif

#define _UNEVT_UnixEventPoll_H_

namespace strmod {
namespace unievent {

/** \class UnixEventPoll UnixEventPoll.h UniEvent/UnixEventPoll.h
 * \brief Manages events associated with various file descriptors and/or
 * signals.
 * This is an implementation of UnixEventRegistry that uses the
 * poll(2) system call to find out about events.
 */
class UnixEventPoll : public UnixEventRegistry {
 public:
   UnixEventPoll(Dispatcher *dispatcher);
   virtual ~UnixEventPoll();

   virtual bool registerFDCond(int fd,
                               const FDCondSet &condbits,
                               const EventPtr &ev) = 0;

   virtual void freeFD(int fd) = 0;

   virtual void onSignal(int signo, const EventPtr &e, bool oneshot = true) = 0;

   virtual void clearSignal(int signo, const EventPtr &e) = 0;

   virtual void clearSignal(int signo) = 0;

   virtual void doPoll(bool wait = true) = 0;
};

} // namespace unievent
} // namespace strmod

#endif
