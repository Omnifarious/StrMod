/* $Header$ */

// $Log$
// Revision 1.1  1998/04/29 01:57:05  hopper
// First cut at making something that can dispatch UNIX system events.
//

#ifdef __GNUG__
#  pragma implementation "UNIXpollManagerImp.h"
#endif

#include "UniEvent/UNIXpollManagerImp.h"
#include "UniEvent/Dispatcher.h"
#include <stddef.h>

UNIXpollManagerImp::UNIXpollManagerImp(UNIDispatcher *disp)
     : UNIXpollManager(disp), poll_list_(NULL), num_entries_(0)
{
}

UNIXpollManagerImp::~UNIXpollManagerImp()
{
   if (poll_list_ != NULL)
   {
      delete [] poll_list_;
      poll_list_ = NULL;
   }
   num_entries_ = 0;
}

bool UNIXpollManagerImp::registerFDCond(int fd, unsigned int condbits,
					const UNIEventPtr &ev)
{
   return(addEV(fd, EVEntry(condbits, false, ev)));
}

bool UNIXpollManagerImp::registerFDCond(int fd, unsigned int condbits,
					const UNIEventPtrT<PollEvent> &ev)
{
   return(addEV(fd, EVEntry(condbits, true, ev)));
}

void UNIXpollManagerImp::freeFD(int fd)
{
   fdmap_.erase(fd);
}

void UNIXpollManagerImp::doPoll()
{
   if (num_entries_ <= 0)
   {
      return;
   }

   fillPollList();

   int result = ::poll(poll_list_, num_entries_, -1);

   if (result > 0)
   {
      for (int i = 0; i < num_entries_; ++i)
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
		  UNIEventPtrT<PollEvent> p;

		  p = static_cast<PollEvent *>((*j).ev_.GetPtr());
		  if (p)
		  {
		     p->setCondBits(condmask & (*j).condmask_);
		  }
	       }
	       getDispatcher()->AddEvent((*j).ev_);
	       j = info.evlist_.erase(j);
	    }
	    else
	    {
	       ++j;
	    }
	 }
      }
   }
}

bool UNIXpollManagerImp::addEV(int fd, const EVEntry &ev_ent)
{
   FDInfo &info = fdmap_[fd];

   info.evlist_.push_back(ev_ent);
   info.condmask_ |= ev_ent.condmask_;
   return(true);
}

void UNIXpollManagerImp::fillPollList()
{
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
   }
}
