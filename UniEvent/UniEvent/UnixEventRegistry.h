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

   /** \class PollEvent UNIXpollManager.h UniEvent/UNIXpollManager.h
    * \brief Event that passes back info about what conditions triggered the
    * event.
    * Note that if you use the same event object for more than one set of
    * conditions, the information returned by getCondBits is going to be
    * spurious.
    */
   class PollEvent : public Event {
    public:
      static const UNEVT_ClassIdent identifier;

      inline PollEvent();
      virtual ~PollEvent()                             { }

      inline virtual int AreYouA(const ClassIdent &cid) const;

      //! Sets the condition bit(s) that triggered this event.
      void setCondBits(const FDCondSet &condbits)      { bits_ = condbits; }

      virtual void triggerEvent(Dispatcher *dispatcher = 0) = 0;

    protected:
      virtual const ClassIdent *i_GetIdent() const     { return(&identifier); }

      //! What condition triggered this event?
      const FDCondSet &getCondBits() const             { return(bits_); }

    private:
      FDCondSet bits_;
   };

   static const UNEVT_ClassIdent identifier;

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
   Dispatcher *disp_;
};

//-----------------------------inline functions--------------------------------

inline int UNIXpollManager::PollEvent::AreYouA(const ClassIdent &cid) const
{
   return (identifier == cid) || Event::AreYouA(cid);
}

inline UNIXpollManager::PollEvent::PollEvent()
     : bits_(0)
{
}

inline UNIXpollManager::UNIXpollManager(Dispatcher *dispatcher)
   : disp_(dispatcher)
{
}

inline UNIXpollManager::~UNIXpollManager()
{
}

} // namespace unievent
} // namespace strmod

#endif
