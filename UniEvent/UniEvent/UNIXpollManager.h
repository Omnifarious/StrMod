#ifndef _UNEVT_UNIXpollManager_H_  // -*- mode: c++ ; c-file-style: "hopper" -*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
//
// Revision 1.3  1999/10/21 02:13:06  hopper
// Removed all references to <bool.h> and changed all references to
// bool_val and bool_cst to bool.
//
// Revision 1.2  1998/05/01 11:54:51  hopper
// Made various changes so that the UNIXpollManager could automatically
// register itself to be run when the event queue was empty.
// Also, changes to use bool_val and bool_cst instead of bool so that we
// can move to a paltform that doesn't support bool more easily.
//
// Revision 1.1  1998/04/29 01:57:09  hopper
// First cut at making something that can dispatch UNIX system events.
//

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

namespace strmod {
namespace unievent {

/** \class UNIXpollManager UNIXpollManager.h UniEvent/UNIXpollManager.h
 * \brief Manages events associated with various file descriptors and/or
 * signals.
 */
class UNIXpollManager {
 public:
   //! These enums are intended to be used as bitmasks.
   enum FDConds { FD_Readable = 0x01, FD_Writeable = 0x02, FD_Error = 0x04,
		  FD_Closed = 0x08, FD_Invalid = 0x10 };
/*   enum SIGConds { SIG_Async = 0x01, SIG_Sync = 0x02 }; */
   static const unsigned int all_conds = FD_Readable | FD_Writeable
                                        | FD_Error | FD_Closed | FD_Invalid;

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
      void setCondBits(unsigned int condbits)          { bits_ = condbits; }

      virtual void triggerEvent(Dispatcher *dispatcher = 0) = 0;

    protected:
      virtual const ClassIdent *i_GetIdent() const     { return(&identifier); }

      //! What condition triggered this event?
      unsigned int getCondBits() const                 { return(bits_); }

    private:
      unsigned int bits_;
   };

   static const UNEVT_ClassIdent identifier;

   inline UNIXpollManager(Dispatcher *dispatcher);
   inline virtual ~UNIXpollManager();

   //! Register the event '*ev' to be fired on file descriptor condition true.
   virtual bool registerFDCond(int fd,
			       unsigned int condbits,
			       const EventPtr &ev) = 0;
   /** Register '*ev' to be fired when file descriptor condition true.
    * This function promises to fill in the poll event with information about
    * the conditions causing the event to be triggered.
    */
   virtual bool registerFDCond(int fd,
			       unsigned int condbits,
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
   return(identifier == cid);
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

}; // namespace unievent
}; // namespace strmod
#endif
