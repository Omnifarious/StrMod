/*
 * Copyright 2002-2010 Eric M. Hopper <hopper@omnifarious.org>
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

/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "UnixEventPoll.h"
#endif

#include "UniEvent/UnixEventPoll.h"
#include "UniEvent/EventPtr.h"
#include "UniEvent/Dispatcher.h"
#include "UniEvent/UNIXError.h"
#include <LCore/LCoreError.h>
#include <utility>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

typedef struct sigaction i_sigaction;

namespace strmod {
namespace unievent {

typedef UnixEventPoll::FDCondSet FDCondSet;
typedef i_sigaction local_sigaction;

namespace {

struct pollstruct : public ::pollfd
{
   pollstruct(int cfd, short cevents, short crevents) {
      fd = cfd;
      events = cevents;
      revents = crevents;
   }
};
struct FDEvent {
   EventPtr ev_;
   FDCondSet condset_;

   FDEvent(const EventPtr ev, const FDCondSet &condset)
        : ev_(ev), condset_(condset) { }
};

using ::std::multimap;
using ::std::vector;
typedef multimap<int, FDEvent> FDMap;
typedef vector<pollstruct> PollList;


typedef std::list<EventPtr> sigevtlist;
struct sigdata
{
   bool handler_registered_;
   sigevtlist events_;

   sigdata() : handler_registered_(false) { }
};
typedef std::vector<sigdata> siglist;

} // anonymous namespace

struct UnixEventPoll::Imp
{
   FDMap fdmap_;
   PollList polllist_;

   sigset_t caught_;
   sigset_t handled_;
   sig_atomic_t sigoccurred_;
   siglist hdlrinfo_;

   Imp() : sigoccurred_(false)
   {
      sigemptyset(&caught_);
      sigemptyset(&handled_);
   }
};

namespace {

/** A convenience class for creating a C++ block in which signal
 * delivery is disabled and automatically re-enabled upon exit from
 * the block.
 */
class SigBlockRegion
{
 public:
   /** Block the specified set of signals and remember what signals were previously blocked.
    *
    * @param blockedones The set of signals to block.
    */
   SigBlockRegion(sigset_t &blockedones)
   {
      sigprocmask(SIG_BLOCK, &blockedones, &oldset_);
   }
   /** Restore the blocked signal mask that was in effect before the constructor was called.
   */
   ~SigBlockRegion()
   {
      sigprocmask(SIG_SETMASK, &oldset_, 0);
   }

 private:
   sigset_t oldset_;

   SigBlockRegion(const SigBlockRegion &b);
   void operator =(const SigBlockRegion &b);
};

}

UnixEventPoll::UnixEventPoll(Dispatcher *dispatcher)
     : impl_(*(new Imp)), dispatcher_(dispatcher)
{
}

UnixEventPoll::~UnixEventPoll()
{
   delete &impl_;
}

void UnixEventPoll::registerFDCond(int fd,
                                   const FDCondSet &condbits,
                                   const EventPtr &ev)
{
   impl_.fdmap_.insert(FDMap::value_type(fd, FDEvent(ev, condbits)));
}

void UnixEventPoll::freeFD(int fd)
{
   impl_.fdmap_.erase(fd);
}

void UnixEventPoll::onSignal(int signo, const EventPtr &e)
{
   unsigned int usigno = (signo < 0) ? ~0U : signo;
   if ((usigno >= (sizeof(sigset_t) * 8)) ||
       (usigno >= max_handled_by_S))
   {
      throw std::range_error("signo is too large in UNIXSignalHandler::onSignal");
   }
   else
   {
      if (impl_.hdlrinfo_.size() <= usigno)
      {
         impl_.hdlrinfo_.resize(usigno + 1);
      }
      sigdata &sighdlrinfo = impl_.hdlrinfo_[usigno];
      if (!sighdlrinfo.handler_registered_)
      {
         if (handled_by_S[usigno] && (handled_by_S[usigno] != this))
         {
            throw std::logic_error("Another handler is handling this signal in UnixEventPoll::onSignal");
         }
         handleSignal(usigno);
         sighdlrinfo.handler_registered_ = true;
      }
      sighdlrinfo.events_.push_back(e);
   }
}

namespace {
class equal_evtptr
{
 public:
   equal_evtptr(const EventPtr &compptr) : compptr_(compptr) { }
   bool operator ()(const EventPtr &evtptr) {
      return evtptr == compptr_;
   }

 private:
   const EventPtr &compptr_;
};
}

void UnixEventPoll::clearSignal(int signo, const EventPtr &e)
{
   unsigned int usigno = (signo < 0) ? ~0U : signo;
   if ((usigno >= (sizeof(sigset_t) * 8)) ||
       (usigno >= max_handled_by_S))
   {
      throw std::range_error("signo is too large in UNIXSignalHandler::clearSignal(int, const EventPtr &)");
   }
   else if (impl_.hdlrinfo_.size() > usigno)
   {
      sigdata &sd = impl_.hdlrinfo_[usigno];
      sd.events_.remove_if(equal_evtptr(e));
      if (sd.events_.size() <= 0)
      {
         unHandleSignal(signo);
         sd.handler_registered_ = false;
      }
   }
}

void UnixEventPoll::clearSignal(int signo)
{
   unsigned int usigno = (signo < 0) ? ~0U : signo;
   if ((usigno >= (sizeof(sigset_t) * 8)) ||
       (usigno >= max_handled_by_S))
   {
      throw std::range_error("signo is too large in UNIXSignalHandler::clearSignal(int)");
   }
   else if (impl_.hdlrinfo_.size() > usigno)
   {
      sigdata &sd = impl_.hdlrinfo_[usigno];
      if (sd.handler_registered_)
      {
         sd.events_.clear();
         unHandleSignal(signo);
         impl_.hdlrinfo_[usigno].handler_registered_ = false;
      }
   }
}

namespace {
inline short condmask_to_pollmask(const FDCondSet &condset)
{
   short pollmask = 0;

   if (condset.test(UnixEventRegistry::FD_Readable))
   {
      pollmask |= POLLIN;
   }
   if (condset.test(UnixEventRegistry::FD_Writeable))
   {
      pollmask |= POLLOUT;
   }
   if (condset.test(UnixEventRegistry::FD_Error))
   {
      pollmask |= POLLERR;
   }
   if (condset.test(UnixEventRegistry::FD_Closed))
   {
      pollmask |= POLLHUP;
   }
   if (condset.test(UnixEventRegistry::FD_Invalid))
   {
      pollmask |= POLLNVAL;
   }
   return pollmask;
}

inline const FDCondSet pollmask_to_condmask(short pollmask)
{
   FDCondSet condset;

   if (pollmask & POLLIN)
   {
      condset.set(UnixEventRegistry::FD_Readable);
   }
   if (pollmask & POLLOUT)
   {
      condset.set(UnixEventRegistry::FD_Writeable);
   }
   if (pollmask & POLLERR)
   {
      condset.set(UnixEventRegistry::FD_Error);
   }
   if (pollmask & POLLHUP)
   {
      condset.set(UnixEventRegistry::FD_Closed);
   }
   if (pollmask & POLLNVAL)
   {
      condset.set(UnixEventRegistry::FD_Invalid);
   }
   return condset;
}
}

void UnixEventPoll::doPoll(bool wait)
{
   impl_.polllist_.clear();
   {
      const FDMap::iterator end = impl_.fdmap_.end();
      for (FDMap::iterator i = impl_.fdmap_.begin(); i != end; )
      {
         const int curfd = i->first;
         FDCondSet condset;
         for (; (i != end) && (curfd == i->first); ++i)
         {
            FDEvent &fdev = i->second;
            condset |= fdev.condset_;
         }
         impl_.polllist_.push_back(pollstruct(curfd,
                                              condmask_to_pollmask(condset),
                                              0));
      }
   }
   int myerrno = 0;
   int pollresult = 0;
   do {
      if (postSigEvents())
      {
         wait = false;
      }
      if (wait)
      {
         const absolute_t curtime = currentTime();
         // 1073741 seconds is approximately 2^30 milliseconds
         interval_t waittil = nextExpirationIn(curtime, interval_t(1073741));
         int polltimeout = (waittil.seconds * 1000U) +
            (waittil.nanoseconds / 1000000U);
         pollresult = ::poll(&(impl_.polllist_[0]), impl_.polllist_.size(),
                             polltimeout);
         myerrno = UNIXError::getErrno();
      }
      else
      {
         pollresult = ::poll(&(impl_.polllist_[0]), impl_.polllist_.size(), 0);
         myerrno = UNIXError::getErrno();
      }
      postExpired(currentTime(), dispatcher_);
      if (pollresult >= 0)
      {
         const PollList::iterator end = impl_.polllist_.end();
         PollList::iterator i = impl_.polllist_.begin();
         while ((pollresult > 0) && (i != end))
         {
            const pollstruct &pollval = *i;
            if (pollval.revents)
            {
               const FDCondSet condset = pollmask_to_condmask(pollval.revents);
               const FDCondSet badset({FD_Closed, FD_Invalid});
               --pollresult;
               FDMap::iterator fdcur = impl_.fdmap_.lower_bound(pollval.fd);
               const FDMap::iterator fdend = impl_.fdmap_.upper_bound(pollval.fd);
               while (fdcur != fdend)
               {
                  const FDEvent &curfdev = fdcur->second;
                  if (condset & curfdev.condset_)
                  {
                     dispatcher_->addEvent(curfdev.ev_);
                     impl_.fdmap_.erase(fdcur++);
                  }
                  else if (badset & condset)
                  {
                     impl_.fdmap_.erase(fdcur++);
                  }
                  else
                  {
                     ++fdcur;
                  }
               }
            }
            ++i;
         }
      }
   } while ((pollresult < 0) && (myerrno == EINTR));
}

bool UnixEventPoll::invariant() const
{
   return true;
}

void UnixEventPoll::printState(::std::ostream &os) const
{
   os << "UnixEventPoll(\n";
   const FDMap::iterator end = impl_.fdmap_.end();
   int prevfd = -1;
   for (FDMap::iterator i = impl_.fdmap_.begin(); i != end; ++i)
   {
      const int fd = i->first;
      const FDCondSet &condset = i->second.condset_;

      if (fd != prevfd)
      {
         prevfd = fd;
         os << std::setw(4) << fd << " (";
      }
      else
      {
         os << "     (";
      }
      {
         bool first = true;
         if (condset[UnixEventPoll::FD_Readable])
         {
            if (!first)
            {
               os << " | ";
               first = false;
            }
            os << "FD_Readable";
         }
         if (condset.test(UnixEventRegistry::FD_Writeable))
         {
            if (!first)
            {
               os << " | ";
               first = false;
            }
            os << "FD_Writeable";
         }
         if (condset.test(UnixEventRegistry::FD_Error))
         {
            if (!first)
            {
               os << " | ";
               first = false;
            }
            os << "FD_Error";
         }
         if (condset.test(UnixEventRegistry::FD_Closed))
         {
            if (!first)
            {
               os << " | ";
               first = false;
            }
            os << "FD_Closed";
         }
         if (condset.test(UnixEventRegistry::FD_Invalid))
         {
            if (!first)
            {
               os << " | ";
               first = false;
            }
            os << "FD_Invalid";
         }
      }
      os << ")\n";
   }
   TimerEventTracker::printState(os);
   os << ")";
}

void UnixEventPoll::signalHandler(int signo)
{
   UnixEventPoll *parent = dynamic_cast<UnixEventPoll *>(handled_by_S[signo]);
   if (parent)
   {
      parent->sigOccurred(signo);
   }
   else
   {
      char str[] = "UnixEventPoll::signalHandler - Got bad signal #0x00\n";
      str[49] = "0123456789ABCDEF"[(signo >> 8) & 0x0f];
      str[50] = "0123456789ABCDEF"[signo & 0x0f];
      // Ignore errors or problems in writing to stderr.
      (void)::write(2, str, sizeof(str));
      local_sigaction act;
      act.sa_handler = SIG_DFL;
      sigemptyset(&act.sa_mask);
      act.sa_flags = 0;
      ::sigaction(signo, &act, 0);
   }
}

void UnixEventPoll::handleSignal(int signo)
{
   local_sigaction act;
   act.sa_handler = signalHandler;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   handled_by_S[signo] = this;
   if (::sigaction(signo, &act, 0) != 0)
   {
      const int myerrno = UNIXError::getErrno();
      throw UNIXError("sigaction", myerrno,
                      lcore::LCoreError(LCORE_GET_COMPILERINFO()));
   }
   sigaddset(&impl_.handled_, signo);
}

void UnixEventPoll::unHandleSignal(int signo)
{
   local_sigaction act;
   act.sa_handler = SIG_DFL;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   ::sigaction(signo, &act, 0);
   handled_by_S[signo] = 0;
   sigdelset(&impl_.handled_, signo);
}

void UnixEventPoll::sigOccurred(int signo)
{
   SigBlockRegion blocker(impl_.handled_);
   sigaddset(&impl_.caught_, signo);
   if (!impl_.sigoccurred_)
   {
      impl_.sigoccurred_ = true;
   }
}

bool UnixEventPoll::postSigEvents()
{
   sigset_t caughtnow;
   {
      SigBlockRegion blocker(impl_.handled_);
      if (impl_.sigoccurred_)
      {
         caughtnow = impl_.caught_;
         sigemptyset(&impl_.caught_);
         impl_.sigoccurred_ = false;
      }
      else
      {
         return false;
      }
   }
   bool caughtany = false;
   {
      const siglist::size_type lastel = impl_.hdlrinfo_.size();
      for (siglist::size_type i = 0; i != lastel; ++i)
      {
         if (sigismember(&caughtnow, i))
         {
            sigdata &thissig = impl_.hdlrinfo_[i];
            if (!thissig.handler_registered_)
            {
               thissig.events_.clear();
            }
            else if (thissig.events_.size() <= 0)
            {
               unHandleSignal(i);
               thissig.handler_registered_ = false;
            }
            else
            {
               const sigevtlist::iterator end = thissig.events_.end();
               for (sigevtlist::iterator i = thissig.events_.begin();
                    i != end;
                    ++i)
               {
                  caughtany = true;
                  dispatcher_->addEvent(*i);
               }
            }
         }
      }
   }
   return caughtany;
}

Timer::absolute_t UnixEventPoll::currentTime() const
{
   struct ::timeval tv;
   ::gettimeofday(&tv, 0);
   return absolute_t(tv.tv_sec, 0, tv.tv_usec * 1000U);
}

} // namespace unievent
} // namespace strmod
