#ifndef _UNEVT_UnixEventGenerator_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <LCore/enum_set.h>

#define _UNEVT_UnixEventGenerator_H_

namespace strmod {
namespace unievent {

/** \class UnixEventGenerator UnixEventGenerator.h UniEvent/UnixEventGenerator.h
 * \brief Manages events associated with various file descriptors and/or
 * signals.
 */
class UnixEventGenerator {
 public:
   //! These enums are intended to be used with the enum_set, FDCondSet.
   enum FDConditions { FD_Readable, FD_Writeable,
                       FD_Error, FD_Closed, FD_Invalid };
   //! A set of 0 or more FDConditions.
   typedef enum_set<FDConditions, FD_Readable, FD_Invalid> FDCondSet;
/*   enum SIGConds { SIG_Async = 0x01, SIG_Sync = 0x02 }; */
   static const FDCondSet all_fdconds;

   inline UnixEventGenerator(Dispatcher *dispatcher);
   inline virtual ~UnixEventGenerator();

   //! Register the event '*ev' to be fired on file descriptor condition true.
   virtual bool registerFDCond(int fd,
                               const FDCondSet &condbits,
                               const EventPtr &ev) = 0;
   /** Register '*ev' to be fired when file descriptor condition true.
    * This function promises to fill in the poll event with information about
    * the conditions causing the event to be triggered.
    */
   virtual bool registerFDCond(int fd,
			       const FDCondSet &condbits,
			       const EventPtrT<PollEvent> &ev) = 0;
/*   // Register the event '*ev' to be fired when a signal happens.
   bool_val registerSIGCond(int sig, unsigned int condbits, const EventPtr &ev);
*/

   //! Removes all entries associated with a particular file descriptor.
   virtual void freeFD(int fd) = 0;

   //! Actually call the UNIX poll system call, and dispatch resulting events.
   virtual void doPoll(bool wait = true) = 0;

 protected:
   Dispatcher *getDispatcher() const                { return(disp_); }

 private:
   Dispatcher * const disp_;
};

//-----------------------------inline functions--------------------------------

inline UnixEventGenerator::PollEvent::PollEvent()
     : bits_(0)
{
}

inline UnixEventGenerator::UnixEventGenerator(Dispatcher *dispatcher)
   : disp_(dispatcher)
{
}

inline UnixEventGenerator::~UnixEventGenerator()
{
}

} // namespace unievent
} // namespace strmod

#endif
