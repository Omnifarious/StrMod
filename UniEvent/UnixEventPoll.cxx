/* $Header$ */

// For a log, see ChangeLog

#include "UniEvent/UnixEventPoll.h"
#include "UniEvent/EventPtr.h"
#include "UniEvent/Dispatcher.h"
#include <utility>
#include <map>
#include <sys/poll.h>

namespace strmod {
namespace unievent {
namespace {

typedef UnixEventPoll::FDCondSet FDCondSet;

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

   FDEvent(const EventPtr ev, FDCondSet condset)
        : ev_(ev), condset_(condset) { }
};
typedef ::std::multimap<int, FDEvent> FDMap;
typedef ::std::vector<pollstruct> PollList;


struct UnixEventPoll::Imp
{
   FDMap fdmap_;
   PollList polllist_;
};

UnixEventPoll::UnixEventPoll(Dispatcher *dispatcher)
     : UnixEventRegistry(dispatcher), impl_(*(new Imp))
{
}

UnixEventPoll::~UnixEventPoll()
{
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

void UnixEventPoll::onSignal(int signo, const EventPtr &e, bool oneshot)
{
   throw ::std::logic_error("UnixEventPoll::onSignal unimplemented.");
}

void UnixEventPoll::clearSignal(int signo, const EventPtr &e)
{
}

void UnixEventPoll::clearSignal(int signo)
{
}

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
   int pollresult = ::poll(&(impl_.polllist_[0]), impl_.polllist_.size(),
                                 wait ? -1 : 0);
   const int myerrno = errno;
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
            --pollresult;
            const FDMap::iterator fdend = impl_.fdmap_.upper_bound(pollval.fd);
            FDMap::iterator fdcur = impl_.fdmap_.lower_bound(pollval.fd);
            while (fdcur != fdend)
            {
               const FDEvent &curfdev = fdcur->second;
               if (curfdev.condset_ & condset)
               {
                  getDispatcher()->addEvent(curfdev.ev_);
                  impl_.fdmap_.erase(fdcur++);
               }
               else
               {
                  ++fdcur;
               }
            }
         }
      }
   }
}

} // Anonymous namespace
} // namespace unievent
} // namespace strmod
