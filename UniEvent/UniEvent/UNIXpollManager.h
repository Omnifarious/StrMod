#ifndef _UNEVT_UNIXpollManager_H_  // -*- mode: c++ ; c-file-style: "hopper" -*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.2  1998/05/01 11:54:51  hopper
// Made various changes so that the UNIXpollManager could automatically
// register itself to be run when the event queue was empty.
// Also, changes to use bool_val and bool_cst instead of bool so that we
// can move to a paltform that doesn't support bool more easily.
//
// Revision 1.1  1998/04/29 01:57:09  hopper
// First cut at making something that can dispatch UNIX system events.
//

#include <bool.h>
#ifndef _UNEVT_Event_H_
#  include <UniEvent/Event.h>
#endif
#ifndef _UNEVT_EventPtrT_H_
#  include <UniEvent/EventPtrT.h>
#endif
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_UNIXpollManager_H_

//: Manages events associated with various file descriptors and/or signals.
class UNIXpollManager {
 public:
   // These enums are intended to be used as bitmasks.
   enum FDConds { FD_Readable = 0x01, FD_Writeable = 0x02, FD_Error = 0x04,
		  FD_Closed = 0x08, FD_Invalid = 0x10 };
/*   enum SIGConds { SIG_Async = 0x01, SIG_Sync = 0x02 }; */
   static const unsigned int all_conds = FD_Readable | FD_Writeable
                                        | FD_Error | FD_Closed | FD_Invalid;

   //: Event that passes back info about what conditions triggered the event.
   // Note that if you use the same event object for more than one set of
   // conditions, the information returned by getCondBits is going to be
   // spurious.
   class PollEvent : public UNIEvent {
    public:
      static const UNEVT_ClassIdent identifier;

      inline PollEvent();
      virtual ~PollEvent()                             { }

      inline virtual int AreYouA(const ClassIdent &cid) const;

      //: Sets the condition bit(s) that triggered this event.
      void setCondBits(unsigned int condbits)          { bits_ = condbits; }

      virtual void TriggerEvent(UNIDispatcher *dispatcher = 0) = 0;

    protected:
      virtual const ClassIdent *i_GetIdent() const     { return(&identifier); }

      //: What condition triggered this event?
      unsigned int getCondBits() const                 { return(bits_); }

    private:
      unsigned int bits_;
   };

   static const UNEVT_ClassIdent identifier;

   inline UNIXpollManager(UNIDispatcher *dispatcher);
   inline virtual ~UNIXpollManager();

   //: Register the event '*ev' to be fired on file descriptor condition true.
   virtual bool_val registerFDCond(int fd,
				   unsigned int condbits,
				   const UNIEventPtr &ev) = 0;
   //: Register '*ev' to be fired when file descriptor condition true.
   // This function promises to fill in the poll event with information about
   // the conditions causing the event to be triggered.
   virtual bool_val registerFDCond(int fd,
				   unsigned int condbits,
				   const UNIEventPtrT<PollEvent> &ev) = 0;
/*   // Register the event '*ev' to be fired when a signal happens.
   bool_val registerSIGCond(int sig, unsigned int condbits, const UNIEventPtr &ev);
*/

   //: Removes all entries associated with a particular file descriptor.
   virtual void freeFD(int fd) = 0;

   //: Actually call the UNIX poll system call, and dispatch resulting events.
   virtual void doPoll() = 0;

 protected:
   UNIDispatcher *getDispatcher() const                { return(disp_); }

 private:
   UNIDispatcher *disp_;
};

//-----------------------------inline functions--------------------------------

inline int UNIXpollManager::PollEvent::AreYouA(const ClassIdent &cid) const
{
   return(identifier == cid);
}

inline UNIXpollManager::PollEvent::PollEvent()
     : bits_(0)
{
}

inline UNIXpollManager::UNIXpollManager(UNIDispatcher *dispatcher)
   : disp_(dispatcher)
{
}

inline UNIXpollManager::~UNIXpollManager()
{
}

#endif
