#ifndef _UNEVT_TimerEventTracker_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <UniEvent/Timer.h>

#define _UNEVT_TimerEventTracker_H_

namespace strmod {
namespace unievent {

/** \class TimerEventTracker TimerEventTracker.h UniEvent/TimerEventTracker.h
 * \brief Tracks Timer events, can be used to implement Timer
 */
class TimerEventTracker : virtual public Timer
{
};

} // namespace unievent
} // namespace strmod

#endif
