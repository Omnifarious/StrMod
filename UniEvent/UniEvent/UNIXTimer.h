#ifndef _UNEVT_UNIXTimer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <UniEvent/Timer.h>

#define _UNEVT_UNIXTimer_H_

namespace strmod {
namespace unievent {

class UNIXSignalHandler;

class UNIXTimer : public Timer
{
 public:
   static const UNEVT_ClassIdent identifier;

   /** Construct a UNIXTimer using a given dispatcher and signal handler
    * Currently this class mainly works with ALRM signal based timers.
    */
   UNIXTimer(Dispatcher &dispatcher, UNIXSignalHandler &handler);
   virtual ~UNIXTimer();

   virtual int AreYouA(const ClassIdent &cid) const {
      return (identifier == cid) || Timer::AreYouA(cid);
   }

   virtual void postAt(const absolute_t &t, const EventPtr &ptr);
   virtual void postIn(const interval_t &off, const EventPtr &ev);

   void checkAlarms()                                   { i_checkAlarms(true); }

 protected:
   inline virtual const ClassIdent *i_GetIdent() const  { return &identifier; }

 private:
   class Imp;
   UNIXSignalHandler &sighand_;
   Timer::interval_t next_;  //!< Interval from bot_ to next event
   Imp &imp_;

   void i_checkAlarms(bool resetalarm);
   void resetAlarm(const interval_t &asofthis);
};

};  // namespace unievent
};  // namespace strmod

#endif
