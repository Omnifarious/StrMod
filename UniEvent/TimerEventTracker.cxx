/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "TimerEventTracker.h"
#endif

#include "UniEvent/TimerEventTracker.h"
#include "UniEvent/EventPtr.h"
#include "UniEvent/Dispatcher.h"
#include <map>
#include <ctime>
#include <stdexcept>

namespace strmod {
namespace unievent {

typedef Timer::interval_t interval_t;
typedef Timer::absolute_t absolute_t;

namespace {
struct compare_intervals
{
   inline bool operator ()(const interval_t &a, const interval_t &b) const;
};

inline bool
compare_intervals::operator ()(const interval_t &a, const interval_t &b) const
{
   return (a.seconds < b.seconds) || (a.nanoseconds < b.nanoseconds);
}

}

typedef std::multimap<interval_t, EventPtr, compare_intervals> timerheap_t;

class TimerEventTracker::Imp
{
 public:
   timerheap_t map1_;
   timerheap_t map2_;
   bool map1_current_;

   Imp() : map1_current_(true) {}
};

TimerEventTracker::TimerEventTracker()
     : old_base_(::time(0)), current_base_(old_base_),
       base_diff_(0, 0), impl_(*new Imp)
{
}

TimerEventTracker::TimerEventTracker(const absolute_t &now)
     : old_base_(now.time), current_base_(old_base_),
       base_diff_(0, 0), impl_(*new Imp)
{
}

TimerEventTracker::~TimerEventTracker()
{
   delete &impl_;
}

void TimerEventTracker::postAt(const absolute_t &t, const EventPtr &ev)
{
   timerheap_t &theheap = impl_.map1_current_ ? impl_.map1_ : impl_.map2_;
   absolute_t curbase = absolute_t(current_base_);
   interval_t timexp = (curbase < t) ? (t - curbase) : interval_t(0, 0);
   theheap.insert(timerheap_t::value_type(timexp, ev));
}

unsigned int
TimerEventTracker::postExpired(const absolute_t &now, Dispatcher *postto)
{
   if (now < current_base_)
   {
      throw ::std::range_error("TimerEventTracker::postExpired: The now "
                               "parameter is not >= than all previous now "
                               "parameters.");
   }
   unsigned int numposted = 0;
   timerheap_t &theheap = impl_.map1_current_ ? impl_.map1_ : impl_.map2_;
   timerheap_t &theoldheap = impl_.map1_current_ ? impl_.map2_ : impl_.map1_;
   {
      const timerheap_t::iterator curend = theheap.upper_bound(now - current_base_);
      const timerheap_t::iterator oldend = (theoldheap.size() <= 0) ?
         theoldheap.begin() :
         theoldheap.upper_bound(now - old_base_);
      timerheap_t::iterator curi = theheap.begin();
      timerheap_t::iterator oldi = theoldheap.begin();
      while ((curi != curend) || (oldi != oldend))
      {
         bool postold = true;
         if (oldi == oldend)
         {
            postold = false;
         }
         else if ((curi != curend) && (curi->first < (oldi->first + base_diff_)))
         {
            postold = false;
         }
         if (postold)
         {
            postto->addEvent(oldi->second);
            ++oldi;
            ++numposted;
         }
         else
         {
            postto->addEvent(curi->second);
            ++curi;
            ++numposted;
         }
      }
      theheap.erase(theheap.begin(), curend);
      theoldheap.erase(theoldheap.begin(), oldend);
   }
   if ((now.time != current_base_) &&
       (now - current_base_).seconds >= (7U * 24U * 60U * 60U))
   {
      const timerheap_t::iterator oldend = theoldheap.end();
      for (timerheap_t::iterator i = theoldheap.begin(); i != oldend; ++i)
      {
         theheap.insert(timerheap_t::value_type(i->first + base_diff_, i->second));
      }
      theoldheap.clear();
      old_base_ = current_base_;
      current_base_ = now.time;
      base_diff_ = absolute_t(current_base_) - absolute_t(old_base_);
      impl_.map1_current_ = !impl_.map1_current_;
   }
   return numposted;
}

Timer::interval_t
TimerEventTracker::nextExpirationIn(const absolute_t &now,
                                    const interval_t &maxtime) const
{
   if (now < current_base_)
   {
      throw ::std::range_error("TimerEventTracker::nextExpirationIn: The now "
                               "parameter is not >= than all previous now "
                               "parameters.");
   }
   timerheap_t &curheap = impl_.map1_current_ ? impl_.map1_ : impl_.map2_;
   timerheap_t &oldheap = impl_.map1_current_ ? impl_.map2_ : impl_.map1_;
   absolute_t earliest = now;
   if (curheap.size() > 0)
   {
      earliest = absolute_t(current_base_, curheap.begin()->second);
      if (oldheap.size() > 0)
      {
         absolute_t contender = absolute_t(old_base_, oldheap.begin()->second);
         if (contender < earliest)
         {
            earliest = contender;
         }
      }
   }
   else if (oldheap.size() > 0)
   {
      earliest = absolute_t(old_base_, oldheap.begin()->second);
   }
   else
   {
      return maxtime;
   }
   if (now < earliest)
   {
      return earliest - now;
   }
   else
   {
      return interval_t(0, 0);
   }
}

} // namespace unievent
} // namespace strmod
