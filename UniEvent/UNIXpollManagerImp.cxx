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

/* $Header$ */

// For a log, see ./ChangeLog

// $Revision$

#ifdef __GNUG__
#  pragma implementation "UNIXpollManagerImp.h"
#endif

#include "UniEvent/Event.h"
#include "UniEvent/UNIXpollManagerImp.h"
#include "UniEvent/Dispatcher.h"
#include <cerrno>
#include <cstddef>

namespace strmod {
namespace unievent {

const UNEVT_ClassIdent UNIXpollManagerImp::identifier(9UL);

class UNIXpollManagerImp::DoPollEvent : public Event
{
 public:
   inline DoPollEvent(UNIXpollManagerImp &parent, bool wait);

   virtual void triggerEvent(Dispatcher *dispatcher = 0);

   inline void managerDeleted();

 private:
   UNIXpollManagerImp *parent_;
   const bool wait_;
};

UNIXpollManagerImp::DoPollEvent::DoPollEvent(UNIXpollManagerImp &parent,
                                             bool wait)
     : parent_(&parent), wait_(wait)
{
}

inline void UNIXpollManagerImp::DoPollEvent::triggerEvent(Dispatcher *dis)
{
   if (parent_ != NULL)
   {
      if (wait_)
      {
         parent_->isregistered_ = false;
         parent_->doPoll(true);
      }
      else
      {
         parent_->isbusyregistered_ = false;
         parent_->doPoll(false);
      }
   }
}

inline void UNIXpollManagerImp::DoPollEvent::managerDeleted()
{
   parent_ = NULL;
}

UNIXpollManagerImp::UNIXpollManagerImp(Dispatcher *disp)
     : UNIXpollManager(disp),
       isregistered_(false),
       isbusyregistered_(false),
       poll_list_(NULL),
       pollev_(new DoPollEvent(*this, true)),
       busypollev_(new DoPollEvent(*this, false)),
       num_entries_(0),
       used_entries_(0)
{
}

UNIXpollManagerImp::~UNIXpollManagerImp()
{
   pollev_.ReleasePtr();
   if (poll_list_ != NULL)
   {
      delete [] poll_list_;
      poll_list_ = NULL;
   }
   num_entries_ = 0;
}

bool UNIXpollManagerImp::registerFDCond(int fd, unsigned int condbits,
					const EventPtr &ev)
{
   if ((condbits & all_conds) != 0)
   {
      return(addEV(fd, EVEntry(condbits, false, ev)));
   }
   else
   {
      // We've 'added' it, it just won't ever get fired, so there's no sense
      // in actually sticking it on our list, except for the ugly and
      // pathological (hence unsupported) case of someone using the
      // destruction of the event object as a trigger for something else.
      return(true);
   }
}

bool UNIXpollManagerImp::registerFDCond(int fd, unsigned int condbits,
					const EventPtrT<PollEvent> &ev)
{
   if ((condbits & all_conds) != 0)
   {
      return(addEV(fd, EVEntry(condbits, true, ev)));
   }
   else
   {
      // We've 'added' it, it just won't ever get fired, so there's no sense
      // in actually sticking it on our list, except for the ugly and
      // pathological (hence unsupported) case of someone using the
      // destruction of the event object as a trigger for something else.
      return(true);
   }
}

void UNIXpollManagerImp::freeFD(int fd)
{
   fdmap_.erase(fd);
}

void UNIXpollManagerImp::doPoll(bool wait)
{
   if (fdmap_.size() <= 0)
   {
      return;
   }

   fillPollList();

   int pollresult;
   if (wait)
   {
      pollresult = ::poll(poll_list_, used_entries_, -1);
   }
   else
   {
      pollresult = ::poll(poll_list_, used_entries_, 0);
   }
   int myerrno = errno;  // Save any error that might have happened.

   if (pollresult >= 0)
   {
      for (unsigned int i = 0; i < used_entries_; ++i)
      {
	 FDInfo &info = fdmap_[poll_list_[i].fd];
	 unsigned long condmask = pevMaskToFDMask(poll_list_[i].revents);
	 EVList::iterator j = info.evlist_.begin();
	 EVList::iterator last = info.evlist_.end();

	 while (j != last)
	 {
	    if ((*j).condmask_ & condmask)
	    {
	       if ((*j).isPoll_)
	       {
		  EventPtrT<PollEvent> p;

		  p = static_cast<PollEvent *>((*j).ev_.GetPtr());
		  if (p)
		  {
		     p->setCondBits(condmask & (*j).condmask_);
		  }
	       }
	       getDispatcher()->addEvent((*j).ev_);
	       j = info.evlist_.erase(j);
	    }
	    else
	    {
	       ++j;
	    }
	 }
	 if ((condmask & FD_Invalid) || (info.evlist_.size() <= 0))
	 {
	    freeFD(poll_list_[i].fd);
	 }
	 else
	 {
	    info.condmask_ = 0;

	    j = info.evlist_.begin();
	    last = info.evlist_.end();
	    for (; j != last; ++j)
	    {
	       info.condmask_ |= (*j).condmask_;
	    }
	 }
      }
   }
   // Only re-register if things were OK, or I got an error I can deal with.
   // Currently, the only error I deal with is being interrupted by a system
   // call.
   if ((pollresult >= 0) || (myerrno == EINTR))
   {
      if (fdmap_.size() > 0)
      {
         if (!isregistered_)
         {
            getDispatcher()->onQueueEmpty(pollev_);
            isregistered_ = true;
         }
         if (!isbusyregistered_)
         {
            getDispatcher()->addBusyPollEvent(busypollev_);
            isbusyregistered_ = true;
         }
      }
   }
}

bool UNIXpollManagerImp::addEV(int fd, const EVEntry &ev_ent)
{
   FDInfo &info = fdmap_[fd];

   info.evlist_.push_back(ev_ent);
   info.condmask_ |= ev_ent.condmask_;
   if (!isregistered_)
   {
      getDispatcher()->onQueueEmpty(pollev_);
      isregistered_ = true;
   }
   if (!isbusyregistered_)
   {
      getDispatcher()->addBusyPollEvent(busypollev_);
      isbusyregistered_ = true;
   }
   return(true);
}

void UNIXpollManagerImp::fillPollList()
{
   used_entries_ = 0;

   if (fdmap_.size() > num_entries_)
   {
      num_entries_ = fdmap_.size();
      if (poll_list_ != NULL)
      {
	 delete [] poll_list_;
	 poll_list_ = NULL;
      }
      poll_list_ = new pollfd[num_entries_];
   }

   {
      FDMap::iterator i = fdmap_.begin();
      FDMap::iterator last = fdmap_.end();
      int j = 0;

      for (; i != last; ++i, ++j)
      {
	 int fd = (*i).first;

	 poll_list_[j].fd = fd;
	 poll_list_[j].events = fdMaskToPEVMask((*i).second.condmask_);
	 poll_list_[j].revents = 0;
      }
      used_entries_ = j;
   }
}

}; // namespace unievent
}; // namespace strmod
