#ifndef _UNEVT_UNIXpollManagerImp_H_  // -*-mode: c++ ; c-file-style: "hopper" -*-

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
// Revision 1.4  1999/10/21 02:13:06  hopper
// Removed all references to <bool.h> and changed all references to
// bool_val and bool_cst to bool.
//
// Revision 1.3  1999/07/20 05:36:33  hopper
// Added a #define of __USE_XOPEN to ensure that Linux defines the full set
// of poll flags.
//
// Revision 1.2  1998/05/01 11:54:52  hopper
// Made various changes so that the UNIXpollManager could automatically
// register itself to be run when the event queue was empty.
// Also, changes to use bool_val and bool_cst instead of bool so that we
// can move to a paltform that doesn't support bool more easily.
//
// Revision 1.1  1998/04/29 01:57:10  hopper
// First cut at making something that can dispatch UNIX system events.
//

#ifndef _UNEVT_UNIXpollManager_H_
#  include <UniEvent/UNIXpollManager.h>
#endif
#ifndef _UNEVT_EventPtr_H_
#  include <UniEvent/EventPtr.h>
#endif
#ifndef _UNEVT_EventPtrT_H_
#  include <UniEvent/EventPtrT.h>
#endif
#include <map>
#include <list>
#define __USE_XOPEN
#include <poll.h>
#undef __USE_XOPEN

#define _UNEVT_UNIXpollManagerImp_H_

namespace strmod {
namespace unievent {

//: The implementation of UNIXpollManager.
// This is seperated out like this so the implementation can change without
// having to recompile the clients of UNIXpollManager.
class UNIXpollManagerImp : public UNIXpollManager {
 public:
   static const UNEVT_ClassIdent identifier;

   UNIXpollManagerImp(Dispatcher *disp);
   virtual ~UNIXpollManagerImp();

   //: See base class
   virtual bool registerFDCond(int fd,
			       unsigned int condbits,
			       const EventPtr &ev);
   //: See base class
   virtual bool registerFDCond(int fd,
			       unsigned int condbits,
			       const EventPtrT<PollEvent> &ev);
   //: See base class
   virtual void freeFD(int fd);

   //: See base class
   virtual void doPoll(bool wait = true);

 private:
   struct EVEntry {
      unsigned int condmask_;
      bool isPoll_;
      EventPtr ev_;

      inline EVEntry(unsigned int condmask,
		     bool isPoll, const EventPtr &ev);
   };
   typedef std::list<EVEntry> EVList;
   struct FDInfo {
      unsigned int condmask_;
      EVList evlist_;

      inline FDInfo();
   };
   typedef std::map<int, FDInfo> FDMap;
   class DoPollEvent;
   friend class DoPollEvent;

   //: Adds an event to the table.
   bool addEV(int fd, const EVEntry &ev_ent);

   //: Fills up the poll_list_ with data so poll can be called.
   void fillPollList();

   //: Converts our condition mask to one suitable for a pollfd structure.
   inline short fdMaskToPEVMask(unsigned int condmask);

   //: Converts a pollfd mask to our condition mask.
   inline unsigned int pevMaskToFDMask(short revents);

   FDMap fdmap_;
   bool isregistered_;
   bool isbusyregistered_;
   pollfd *poll_list_;
   EventPtrT<DoPollEvent> pollev_;
   EventPtrT<DoPollEvent> busypollev_;
   unsigned int num_entries_;
   unsigned int used_entries_;
};

//-----------------------------inline functions--------------------------------

inline UNIXpollManagerImp::EVEntry::EVEntry(unsigned int condmask,
					    bool isPoll,
					    const EventPtr &ev)
     : condmask_(condmask), isPoll_(isPoll), ev_(ev)
{
}

inline UNIXpollManagerImp::FDInfo::FDInfo()
     : condmask_(0)
{
}

inline short UNIXpollManagerImp::fdMaskToPEVMask(unsigned int condmask)
{
   short events = 0;

   if (condmask & FD_Readable)
   {
      events |= POLLRDNORM;
   }
   if (condmask & FD_Writeable)
   {
      events |= POLLOUT;
   }
   return(events);
}

inline unsigned int UNIXpollManagerImp::pevMaskToFDMask(short revents)
{
   unsigned int condmask = 0;

   if (revents & POLLRDNORM)
   {
      condmask |= FD_Readable;
   }
   if (revents & POLLOUT)
   {
      condmask |= FD_Writeable;
   }
   if (revents & POLLERR)
   {
      condmask |= FD_Error;
   }
   if (revents & POLLHUP)
   {
      condmask |= FD_Closed;
   }
   if (revents & POLLNVAL)
   {
      condmask |= FD_Invalid;
   }
   return(condmask);
}

}; // namesapce unievent
}; // namespace strmod

#endif
