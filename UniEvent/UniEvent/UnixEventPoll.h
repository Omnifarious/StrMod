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
class UnixEventPoll : public UnixEventRegistry
{
 public:
   UnixEventPoll(Dispatcher *dispatcher);
   virtual ~UnixEventPoll();

   virtual void registerFDCond(int fd,
                               const FDCondSet &condbits,
                               const EventPtr &ev);

   virtual void freeFD(int fd);

   virtual void onSignal(int signo, const EventPtr &e, bool oneshot = true);

   virtual void clearSignal(int signo, const EventPtr &e);

   virtual void clearSignal(int signo);

   virtual void doPoll(bool wait = false);

 private:
   struct Imp;

   Imp &impl_;
};

} // namespace unievent
} // namespace strmod

#endif
