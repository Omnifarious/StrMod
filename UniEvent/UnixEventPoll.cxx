/* $Header$ */

// For a log, see ChangeLog

#include "UniEvent/UnixEventPoll.h"
#include "UniEvent/EventPtr.h"
#include <utility>
#include <map>
#include <sys/poll.h>

namespace strmod {
namespace unievent {
namespace {

typedef UnixEventPoll::FDCondSet FDCondSet;

typedef ::pollfd pollstruct;
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
      }
   }
}

}
}
}
