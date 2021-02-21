#ifndef _UNEVT_TimerEventTracker_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog

#include <UniEvent/Timer.h>
#include <LCore/Debugable.h>

#define _UNEVT_TimerEventTracker_H_

namespace strmod {
namespace unievent {

class Dispatcher;

/** \class TimerEventTracker TimerEventTracker.h UniEvent/TimerEventTracker.h
 * \brief Tracks Timer events, can be used to implement Timer
 */
class TimerEventTracker : virtual public Timer, virtual public lcore::Debugable
{
 public:
   /** \brief Construct using the ANSI C time function to provide an initial
    * base.
    */
   TimerEventTracker();
   //! Construct using \c now as an initial base.
   explicit TimerEventTracker(const absolute_t &now);
   //! Nothing special or unexpected
   virtual ~TimerEventTracker();

   void postAt(const absolute_t &t, const EventPtr &ev) override;
   // Leave the implementation of postIn as it is in Timer.
   absolute_t currentTime() const override = 0;

   /** Post all expired timer events.
    * @param now What you want the current time to be for the purposes of expiration
    * @param postto The Dispatcher to post the events to.
    * @return The number of events posted.
    *
    * Each call of this function must have a \c now parameter that is >= the \c
    * now parameter given in the previous call.  In other words, the \c now
    * parameter must monotonically increase in subsequent calls.
    *
    * Also, the first \c now must be >= the \c now you gave in the constructor,
    * or the value returned by the time() ANSI C call at construction time if
    * you didn't give a value in the constructor.
    */
   unsigned int postExpired(const absolute_t &now, Dispatcher *postto);

   /** Return the time of the next timer expiration, or maxtime.
    * @param now What time to calculate the next expiration from.
    * @param maxtime The maximum interval to return.
    * @return The internval from now until the next expiration.
    * If there is no timer scheduled to expire, or it's farther than maxtime
    * into the future, maxtime is returned.
    */
   interval_t nextExpirationIn(const absolute_t &now,
                               const interval_t &maxtime) const;

   bool invariant() const override                               { return true; }
   void printState(::std::ostream &os) const override;

 private:
   class Imp;

   time_t old_base_;
   time_t current_base_;
   interval_t base_diff_;
   Imp &impl_;
};

//-----------------------------inline functions--------------------------------

} // namespace unievent
} // namespace strmod

#endif
