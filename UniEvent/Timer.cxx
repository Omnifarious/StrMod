/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "Timer.h"
#endif

#include "UniEvent/Timer.h"
#include <cmath>

namespace strmod {
namespace unievent {

const UNEVT_ClassIdent Timer::identifier(12UL);

bool operator <(const Timer::interval_t &a, const Timer::interval_t &b)
{
   Timer::interval_t an(a);
   an.normalize();
   Timer::interval_t bn(b);
   bn.normalize();
   if (an.seconds != bn.seconds)
   {
      return an.seconds < bn.seconds;
   }
   else
   {
      return an.nanoseconds < bn.nanoseconds;
   }
}

bool operator ==(const Timer::interval_t &a, const Timer::interval_t &b)
{
   Timer::interval_t an(a);
   an.normalize();
   Timer::interval_t bn(b);
   bn.normalize();
   if (an.seconds != bn.seconds)
   {
      return an.seconds < bn.seconds;
   }
   else
   {
      return an.nanoseconds < bn.nanoseconds;
   }
}

//--

bool operator <(const Timer::absolute_t &a, const Timer::absolute_t &b)
{
   double seconds = ::difftime(a.time, b.time);
   if (seconds == 0)
   {
      const Timer::interval_t &ainterval = a;
      const Timer::interval_t &binterval = b;
      return ainterval <= binterval;
   }
   else if (seconds < 0)  // a.time < b.time
   {
      seconds = -seconds;
      double disecs;
      unsigned long insecs;
      insecs = static_cast<unsigned long>(::floor(modf(seconds, &disecs) * 1000000000 + 0.5));
      unsigned long isecs = static_cast<unsigned long>(disecs);
      // Add the interval between a.time and b.time to binterval.
      // Essentially represent both a and b as intervals from a.time.
      Timer::interval_t binterval(b);
      binterval.normalize();
      binterval.seconds += isecs;
      binterval.nanoseconds += insecs;
      const Timer::interval_t &ainterval = a;
      return ainterval < binterval;
   }
   else // a.time >= b.time
   {
      double disecs;
      unsigned long insecs;
      insecs = static_cast<unsigned long>(::floor(modf(seconds, &disecs) * 1000000000 + 0.5));
      unsigned long isecs = static_cast<unsigned long>(disecs);
      // Add the interval between a.time and b.time to ainterval.
      // Essentially represent both a and b as intervals from b.time.
      Timer::interval_t ainterval(a);
      ainterval.normalize();
      ainterval.seconds += isecs;
      ainterval.nanoseconds += insecs;
      const Timer::interval_t &binterval = b;
      return ainterval < binterval;
   }
}

bool operator ==(const Timer::absolute_t &a, const Timer::absolute_t &b)
{
   double seconds = ::difftime(a.time, b.time);
   if (seconds == 0)
   {
      const Timer::interval_t &ainterval = a;
      const Timer::interval_t &binterval = b;
      return ainterval == binterval;
   }
   else if (seconds < 0)  // a.time < b.time
   {
      seconds = -seconds;
      double disecs;
      unsigned long insecs;
      insecs = static_cast<unsigned long>(::floor(modf(seconds, &disecs) * 1000000000 + 0.5));
      unsigned long isecs = static_cast<unsigned long>(disecs);
      // Add the interval between a.time and b.time to binterval.
      // Essentially represent both a and b as intervals from a.time.
      Timer::interval_t binterval(b);
      binterval.normalize();
      binterval.seconds += isecs;
      binterval.nanoseconds += insecs;
      const Timer::interval_t &ainterval = a;
      return ainterval == binterval;
   }
   else // a.time >= b.time
   {
      double disecs;
      unsigned long insecs;
      insecs = static_cast<unsigned long>(::floor(modf(seconds, &disecs) * 1000000000 + 0.5));
      unsigned long isecs = static_cast<unsigned long>(disecs);
      // Add the interval between a.time and b.time to ainterval.
      // Essentially represent both a and b as intervals from b.time.
      Timer::interval_t ainterval(a);
      ainterval.normalize();
      ainterval.seconds += isecs;
      ainterval.nanoseconds += insecs;
      const Timer::interval_t &binterval = b;
      return ainterval == binterval;
   }
}

const Timer::interval_t
operator -(const Timer::interval_t &a, const Timer::interval_t &b)
{
   Timer::interval_t an = a;
   an.normalize();
   Timer::interval_t bn = b;
   bn.normalize();
   if (an.seconds  > bn.seconds)
   {
      unsigned long secdiff = an.seconds - bn.seconds;
      if (an.nanoseconds >= bn.nanoseconds)
      {
         return Timer::interval_t(secdiff, an.nanoseconds - bn.nanoseconds);
      }
      else
      {
         return Timer::interval_t(secdiff - 1,
                                  (an.nanoseconds + 1000000000U) -
                                  bn.nanoseconds);
      }
   }
   else if (an.seconds == bn.seconds)
   {
      if (an.nanoseconds >= bn.nanoseconds)
      {
         return Timer::interval_t(0, an.nanoseconds - bn.nanoseconds);
      }
      else
      {
         return Timer::interval_t(0, bn.nanoseconds - an.nanoseconds);
      }
   }
   else // an.seconds < bn.seconds
   {
      unsigned long secdiff = bn.seconds - an.seconds;
      if (bn.nanoseconds >= an.nanoseconds)
      {
         return Timer::interval_t(secdiff, bn.nanoseconds - an.nanoseconds);
      }
      else
      {
         return Timer::interval_t(secdiff - 1,
                                  (bn.nanoseconds + 1000000000U) -
                                  an.nanoseconds);
      }
   }
}

};  // namespace unievent
};  // namespace strmod