#ifndef _UNEVT_UnixEventPoll_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#ifndef _UNEVT_UnixEventRegistry_H_
#  include <UniEvent/UnixEventRegistry.h>
#endif
#ifndef _UNEVT_Timer_H_
#  include <UniEvent/Timer.h>
#endif
#include <LCore/Debugable.h>

#define _UNEVT_UnixEventPoll_H_

namespace strmod {
namespace unievent {

/** \class UnixEventPoll UnixEventPoll.h UniEvent/UnixEventPoll.h
 * \brief Manages events associated with various file descriptors and/or
 * signals.
 * This is an implementation of UnixEventRegistry that uses the
 * poll(2) system call to find out about events.
 */
class UnixEventPoll : virtual public UnixEventRegistry,
                      virtual public Debugable,
                      virtual public Timer
{
 public:
   UnixEventPoll(Dispatcher *dispatcher);
   virtual ~UnixEventPoll();

   virtual bool invariant() const;
   virtual void printState(::std::ostream &os) const;

   virtual void registerFDCond(int fd,
                               const FDCondSet &condbits,
                               const EventPtr &ev);

   virtual void freeFD(int fd);

   virtual void onSignal(int signo, const EventPtr &e);

   virtual void clearSignal(int signo, const EventPtr &e);

   virtual void clearSignal(int signo);

   virtual void postAt(const absolute_t &t, const EventPtr &ev);

   virtual void postIn(const interval_t &off, const EventPtr &ev);

   virtual void doPoll(bool wait = false);

 private:
   struct Imp;

   Imp &impl_;
   Dispatcher * const dispatcher_;

   static void signalHandler(int signo);
   void handleSignal(int signo);
   void unHandleSignal(int signo);
   void sigOccurred(int signo);
   bool postSigEvents();
};

} // namespace unievent
} // namespace strmod

#endif
