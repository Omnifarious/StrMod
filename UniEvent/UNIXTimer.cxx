/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "UNIXTimer.h"
#endif

#include "UniEvent/UNIXTimer.h"
#include "UniEvent/UNIXSignalHandler.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include "UniEvent/Dispatcher.h"
#include <multimap.h>
#include <functional>
#include <ctime>
#include <sys/time.h>
#include <signal.h>

namespace strmod {
namespace unievent {
namespace {

const UNEVT_ClassIdent UNIXTimer::identifier(13UL);

inline bool norm_lessthan(const Timer::interval_t &a,
                          const Timer::interval_t &b)
{
   return (a.seconds < b.seconds) ||
      ((a.seconds == b.seconds) && (a.nanoseconds < b.nanoseconds));
}

class norm_int_lessthan : public std::binary_function<const Timer::interval_t &,
                          const Timer::interval_t &, bool>
{
 public:
   inline bool operator ()(const Timer::interval_t &a,
                           const Timer::interval_t &b)
   {
      return norm_lessthan(a, b);
   }
};

typedef std::multimap<Timer::interval_t, EventPtr, norm_int_lessthan> SortedIntervals;

class TimerEvent : public Event {
   friend class UNIXTimer;
 public:
   TimerEvent(UNIXTimer &parent);
   virtual ~TimerEvent();

   virtual void triggerEvent(Dispatcher *dispatcher);

 private:
   void forgetParent();

   UNIXTimer *parent_;
};

TimerEvent::TimerEvent(UNIXTimer &parent)
     : parent_(&parent)
{
}

TimerEvent::~TimerEvent()
{
}

void TimerEvent::triggerEvent(Dispatcher *dispatcher)
{
   if (parent_)
   {
      parent_->checkAlarms();
   }
}

typedef ::timeval timeval;
typedef Timer::time_t time_t;

struct UNIXTimer::Imp
{
   timeval bot_;  //!< Beginning Of Time
   SortedIntervals alarms_;
   EventPtr alarmev_;
};

UNIXTimer::UNIXTimer(Dispatcher &dispatcher, UNIXSignalHandler &handler)
     : Timer(dispatcher), sighand_(handler), imp_(*(new Imp))
{
   ::gettimeofday(&imp_.bot_, 0);
   imp_.alarmev_ = new TimerEvent(*this);
   sighand_.onSignal(SIGALRM, imp_.alarmev_, false);
}

UNIXTimer::~UNIXTimer()
{
   sighand_.removeEvent(SIGALRM, imp_.alarmev_, false);
   delete &imp_;
}

const Timer::interval_t operator -(timeval a, const timeval &b)
{
   if (b.tv_usec > a.tv_usec)
   {
      a.tv_sec -= 1;
      a.tv_usec += 1000000U;
   }
   if (a.tv_sec > b.tv_sec)
   {
      return Timer::interval_t(a.tv_sec - b.tv_sec,
                               (a.tv_usec - b.tv_usec) * 1000U);
   }
   else if ((a.tv_sec == b.tv_sec) && (a.tv_usec > b.tv_usec))
   {
      return Timer::interval_t(0, (a.tv_usec - b.tv_usec) * 1000U);
   }
   else
   {
      return Timer::interval_t(0, 0);
   }
}

inline const Timer::interval_t
absolute_to_offset_from(const Timer::absolute_t &t, const timeval &bot)
{
   timeval a = { t.time + t.seconds, t.nanoseconds / 1000U };
   return a - bot;
}

void UNIXTimer::postAt(const Timer::absolute_t &t, const EventPtr &ptr)
{
   Timer::absolute_t normt(t);
   const Timer::interval_t when = absolute_to_offset_from(normt, imp_.bot_);
   bool wasempty = imp_.alarms_.empty();
   imp_.alarms_.insert(SortedIntervals::value_type(when, ptr));
   if (wasempty || norm_lessthan(when, next_))
   {
      next_ = when;
      i_checkAlarms(true);
   }
   else
   {
      i_checkAlarms(false);
   }
}

void UNIXTimer::postIn(const interval_t &off, const EventPtr &ptr)
{
   timeval tv;
   ::gettimeofday(&tv, 0);
   Timer::interval_t normoff(off);
   normoff.normalize();
   const Timer::interval_t bottonow(tv - imp_.bot_);
   Timer::interval_t absoff(bottonow.seconds + normoff.seconds,
                            bottonow.nanoseconds + normoff.nanoseconds);
   absoff.normalize();
   bool wasempty = imp_.alarms_.empty();
   imp_.alarms_.insert(SortedIntervals::value_type(absoff, ptr));
   if (wasempty || norm_lessthan(absoff, next_))
   {
      next_ = absoff;
      i_checkAlarms(true);
   }
   else
   {
      i_checkAlarms(false);
   }
}

void UNIXTimer::i_checkAlarms(bool resetalarm)
{
   timeval tv;
   ::gettimeofday(&tv, 0);
   const Timer::interval_t bottonow(tv - imp_.bot_);
   if (!norm_lessthan(bottonow, next_))
   {
      while (!imp_.alarms_.empty())
      {
         const SortedIntervals::iterator first = imp_.alarms_.begin();
         if (norm_lessthan(bottonow, first->first))
         {
            next_ = first->first;
            resetalarm = true;
            break;
         }
         else
         {
            getDispatcher().addEvent(first->second);
            imp_.alarms_.erase(first);
         }
      }
      if (imp_.alarms_.empty())
      {
         next_.seconds = 0;
         next_.nanoseconds = 0;
         resetalarm = false;
      }
   }
   if (resetalarm)
   {
      resetAlarm(bottonow);
   }
}

void UNIXTimer::resetAlarm(const interval_t &asofthis)
{
   assert(norm_lessthan(asofthis, next_));
   itimerval ittv = { {0, 0}, {0, 0} };
   timeval &nexttv = ittv.it_value;
   if (next_.seconds <= asofthis.seconds)
   {
      nexttv.tv_sec = 0;
      // Convert nanoseconds to microseconds.
      nexttv.tv_usec = (next_.nanoseconds - asofthis.nanoseconds) / 1000U;
   }
   else
   {
      nexttv.tv_sec = next_.seconds - asofthis.seconds;
      if (next_.nanoseconds >= asofthis.nanoseconds)
      {
         // Convert nanoseconds to microseconds.
         nexttv.tv_usec = (next_.nanoseconds - asofthis.nanoseconds) / 1000U;
      }
      else
      {
         --(nexttv.tv_sec);
         // Convert nanoseconds to microseconds.
         nexttv.tv_usec = ((next_.nanoseconds + 1000000000UL) -
                           asofthis.nanoseconds) / 1000U;
      }
   }
   setitimer(ITIMER_REAL, &ittv, 0);
}

};  // Anonymous namespace
};  // namespace unievent
};  // namespace strmod
