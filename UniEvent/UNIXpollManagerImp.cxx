/* $Header$ */

// For a log, see ./ChangeLog

// $Revision$

#ifdef __GNUG__
#  pragma implementation "UNIXpollManagerImp.h"
#endif

#include "UniEvent/Event.h"
#include "UniEvent/UNIXpollManagerImp.h"
#include "UniEvent/Dispatcher.h"
#include <errno.h>
#include <stddef.h>

const UNEVT_ClassIdent UNIXpollManagerImp::identifier(9UL);

class UNIXpollManagerImp::DoPollEvent : public UNIEvent
{
 public:
   inline DoPollEvent(UNIXpollManagerImp &parent);

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0);

   inline void managerDeleted();

 private:
   UNIXpollManagerImp *parent_;
};

UNIXpollManagerImp::DoPollEvent::DoPollEvent(UNIXpollManagerImp &parent)
     : parent_(&parent)
{
}

inline void UNIXpollManagerImp::DoPollEvent::TriggerEvent(UNIDispatcher *dis)
{
   if (parent_ != NULL)
   {
      parent_->isregistered_ = false;
      parent_->doPoll();
   }
}

inline void UNIXpollManagerImp::DoPollEvent::managerDeleted()
{
   parent_ = NULL;
}

UNIXpollManagerImp::UNIXpollManagerImp(UNIDispatcher *disp)
     : UNIXpollManager(disp),
       poll_list_(NULL),
       num_entries_(0),
       isregistered_(false),
       pollev_(new DoPollEvent(*this))
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
					const UNIEventPtr &ev)
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
					const UNIEventPtrT<PollEvent> &ev)
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

void UNIXpollManagerImp::doPoll()
{
   if (fdmap_.size() <= 0)
   {
      return;
   }

   fillPollList();

   int pollresult = ::poll(poll_list_, used_entries_, -1);
   int myerrno = errno;  // Save any error that might have happened.

   if (pollresult >= 0)
   {
      for (int i = 0; i < used_entries_; ++i)
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
      if (fdmap_.size() > 0 && !isregistered_)
      {
	 getDispatcher()->onQueueEmpty(pollev_);
	 isregistered_ = true;
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
