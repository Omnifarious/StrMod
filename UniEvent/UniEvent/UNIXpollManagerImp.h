#ifndef _UNEVT_UNIXpollManagerImp_H_  // -*-mode: c++ ; c-file-style: "hopper" -*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
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
#include <stropts.h>
#include <poll.h>

#define _UNEVT_UNIXpollManagerImp_H_

//: The implementation of UNIXpollManager.
// This is seperated out like this so the implementation can change without
// having to recompile the clients of UNIXpollManager.
class UNIXpollManagerImp : public UNIXpollManager {
 public:
   UNIXpollManagerImp(UNIDispatcher *disp);
   virtual ~UNIXpollManagerImp();

   //: See base class
   virtual bool registerFDCond(int fd,
			       unsigned int condbits,
			       const UNIEventPtr &ev);
   //: See base class
   virtual bool registerFDCond(int fd,
			       unsigned int condbits,
			       const UNIEventPtrT<PollEvent> &ev);
   //: See base class
   virtual void freeFD(int fd) = 0;

   //: Actually call the UNIX poll system call, and dispatch resulting events.
   virtual void doPoll();

 private:
   struct EVEntry {
      unsigned int condmask_;
      bool isPoll_;
      UNIEventPtr ev_;

      inline EVEntry(unsigned int condmask,
		     bool isPoll, const UNIEventPtr &ev);
   };
   typedef list<EVEntry> EVList;
   struct FDInfo {
      unsigned int condmask_;
      EVList evlist_;

      inline FDInfo();
   };
   typedef map<int, FDInfo> FDMap;

   //: Adds an event to the table.
   bool addEV(int fd, const EVEntry &ev_ent);

   //: Fills up the poll_list_ with data so poll can be called.
   void fillPollList();

   //: Converts our condition mask to one suitable for a pollfd structure.
   inline short fdMaskToPEVMask(unsigned int condmask);

   //: Converts a pollfd mask to our condition mask.
   inline unsigned int pevMaskToFDMask(short revents);

   FDMap fdmap_;
   pollfd *poll_list_;
   unsigned int num_entries_;
};

//-----------------------------inline functions--------------------------------

inline UNIXpollManagerImp::EVEntry::EVEntry(unsigned int condmask,
					    bool isPoll,
					    const UNIEventPtr &ev)
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
}

#endif
