#ifndef _UNEVT_Timer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
// $Revision$
// $Tag$

#include <ctime>
#include <iosfwd>
#include <LCore/GenTypes.h>
#include <LCore/Protocol.h>
#include <UniEvent/UNEVT_ClassIdent.h>

#define _UNEVT_Timer_H_

namespace strmod {
namespace unievent {

class EventPtr;

/** \class Timer Timer.h UniEvent/Timer.h
 * \brief Posts events that happen based on time.
 *
 * The ANSI C standard defines a rather anemic set of types for dealing with
 * time.  Worse, the exact values in most of the types are OS specific.  You
 * cannot count on time_t representing a value in the unit of seconds, for
 * example.
 *
 * Since this is supposed to a generic, OS agnostic interface, this leaves us in
 * something of a quandry.  The ANSI things _do_ tend to be focused on calendar
 * time though, so using our own interval type is perfectly reasonable.
 *
 * Given an interval type, it's reasonable to use the interval as an offset from
 * a time_t without encoding any knowledge of what a time_t actually represents
 * into the interface.
 *
 * It's tempting to use the rather well defined struct tm type until you examine
 * it closely and realize that it contains no provision for representing the
 * timezone.  Since it actually contains time information that would be
 * different in different timezones, this is a fatal lack.  A time_t is
 * sufficiently ambiguous that the questions of whether or not it is UTC or a
 * local time zone, and whether or not this can even be determined in the
 * environment are not important to the interface.
 */
class Timer : public Protocol {
 public:
   /** \class interval_t Timer.h UniEvent/Timer.h
    * \brief A time interval.
    * This interval is of only nanosecond precision.  If you need to time lasers
    * for photographing molecular interactions or schedule oscilloscope sweeps,
    * this class is probably not precise enough for you.  Then again, most OSes
    * are really iffy for anything less than millisecond resolution anyway.
    */
   struct interval_t {
      //@{
      /** These are public and not suffixed with '_' because they are part of
       * the public interface for this class.
       */
      unsigned long seconds;  //!< Seconds
      U4Byte nanoseconds;  //!< Billionths of a second.
      //@}
      inline interval_t(unsigned long secs = 0, U4Byte nanosecs = 0);
      inline void normalize();
   };
   typedef ::time_t time_t;
   struct absolute_t : private interval_t {
      friend bool operator <(const absolute_t &a, const absolute_t &b);
      friend bool operator ==(const absolute_t &a, const absolute_t &b);
      friend const interval_t operator -(const absolute_t &a,
                                         const absolute_t &b);
      friend const absolute_t operator +(const absolute_t &a,
                                         const interval_t &b);
      time_t time;  //!< The OSes idea of an absolute time.
      interval_t::seconds;  //!< Seconds past \c time.
      interval_t::nanoseconds;  //!< Billionths of a second past \c time.

      inline absolute_t(time_t time_param,
                        unsigned long secs = 0, U4Byte nanosecs = 0) :
           interval_t(secs, nanosecs), time(time_param)
      {
      }
      inline absolute_t(time_t time_param, const interval_t &interval) :
           interval_t(interval), time(time_param)
      {
      }
      interval_t::normalize;
   };
   static const UNEVT_ClassIdent identifier;

   /** Construct a Timer
    */
   Timer()                                                       { }
   /** \brief Destroy a Timer.
    * All events and the times they were supposed to happen are forgotten.
    */
   virtual ~Timer()                                              { }

   virtual int AreYouA(const ClassIdent &cid) const {
      return (identifier == cid) || Protocol::AreYouA(cid);
   }

   //! Post an event at a particular time
   virtual void postAt(const absolute_t &t, const EventPtr &ev) = 0;
   //! Post an event after a certain amount of time has expired.
   virtual void postIn(const interval_t &off, const EventPtr &ev) = 0;

 protected:
   inline virtual const ClassIdent *i_GetIdent() const   { return &identifier; }
};

//-----------------------------inline functions--------------------------------

//! Construct a Timer::interval_t possibly from seconds and nanoseconds.
inline Timer::interval_t::interval_t(unsigned long secs, U4Byte nanosecs)
     : seconds(secs), nanoseconds(nanosecs)
{
   normalize();
}

//! Reduce nanoseconds to represent less than a full second.
inline void Timer::interval_t::normalize()
{
   if (nanoseconds >= 1000000000U)
   {
      seconds += (nanoseconds / 1000000000U);
      nanoseconds %= 1000000000U;
   }
}

bool operator <(const Timer::interval_t &a, const Timer::interval_t &b);

inline bool operator >=(const Timer::interval_t &a, const Timer::interval_t &b)
{
   return !(a < b);
}

inline bool operator >(const Timer::interval_t &a, const Timer::interval_t &b)
{
   return b < a;
}

inline bool operator <=(const Timer::interval_t &a, const Timer::interval_t &b)
{
   return !(b < a);
}

bool operator ==(const Timer::interval_t &a, const Timer::interval_t &b);

inline bool operator !=(const Timer::interval_t &a, const Timer::interval_t &b)
{
   return !(a == b);
}

//--

bool operator <(const Timer::absolute_t &a, const Timer::absolute_t &b);

inline bool operator >=(const Timer::absolute_t &a, const Timer::absolute_t &b)
{
   return !(a < b);
}

inline bool operator >(const Timer::absolute_t &a, const Timer::absolute_t &b)
{
   return b < a;
}

inline bool operator <=(const Timer::absolute_t &a, const Timer::absolute_t &b)
{
   return !(b < a);
}

bool operator ==(const Timer::absolute_t &a, const Timer::absolute_t &b);

inline bool operator !=(const Timer::absolute_t &a, const Timer::absolute_t &b)
{
   return !(a == b);
}

//--

//: Difference between two intervals, always >= 0
const Timer::interval_t
operator -(const Timer::interval_t &a, const Timer::interval_t &b);

//: Sum of two intervals
inline const Timer::interval_t
operator +(const Timer::interval_t &a, const Timer::interval_t &b)
{
   Timer::interval_t an = a;
   an.normalize();
   Timer::interval_t bn = b;
   bn.normalize();
   Timer::interval_t retval(an.seconds + bn.seconds,
                            an.nanoseconds + bn.nanoseconds);
   retval.normalize();
   return retval;
}

//: Interval between two absolute times, always >= 0
const Timer::interval_t
operator -(const Timer::absolute_t &a, const Timer::absolute_t &b);

//: Add an interval to an absolute time.
inline const Timer::absolute_t
operator +(const Timer::absolute_t &a, const Timer::interval_t &b)
{
   const Timer::interval_t &aint = a;
   Timer::absolute_t retval(a.time, aint + b);
   return retval;
}

::std::ostream &operator <<(::std::ostream &os, const Timer::absolute_t &time);

::std::ostream &operator <<(::std::ostream &os, const Timer::interval_t &time);

};  // namespace unievent
};  // namespace strmod

#endif
