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

// For a log, see ../ChangeLog
//
// Revision 1.1  1998/05/01 01:14:37  hopper
// Added a new test for the UNIXpollManager.
//

#include "UniEvent/SimpleDispatcher.h"
#include "UniEvent/UNIXpollManagerImp.h"
#include "UniEvent/EventPtrT.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

class MyEvent : public UNIXpollManager::PollEvent {
 public:
   inline MyEvent(int fd, UNIXpollManager &pollman);
   virtual ~MyEvent();

   virtual void triggerEvent(UNIDispatcher *dispatcher);

 private:
   int fd_;
   unsigned int counter_;
   char strbuf_[80];
   size_t bufpos_, buflen_;
   UNIXpollManager &pollman_;
};

inline MyEvent::MyEvent(int fd, UNIXpollManager &pollman)
     : fd_(fd), pollman_(pollman), bufpos_(0), buflen_(0), counter_(0)
{
   sprintf(strbuf_, "%d:%ud\n", fd_, counter_);
   buflen_ = strlen(strbuf_);
   cerr << "Created MyEvent(" << fd_ << ")\n";
}

MyEvent::~MyEvent()
{
   cerr << "Destroying MyEvent(" << fd_
	<< ")  (counter_ == " << counter_ << ")\n";
}

void MyEvent::triggerEvent(UNIDispatcher *dispatcher)
{
   unsigned int condbits = getCondBits();
   unsigned int newcondbits = 0;

   cerr << "MyEvent("<< fd_ << ")::triggerEvent\n";

   if (condbits & UNIXpollManager::FD_Writeable)
   {
      int result;

      result = write(fd_, strbuf_ + bufpos_, buflen_ - bufpos_);
      while (result > 0 && (counter_ < 8192))
      {
	 bufpos_ += result;
	 if (bufpos_ >= buflen_)
	 {
	    sprintf(strbuf_, "%d:%u\n", fd_, ++counter_);
	    buflen_ = strlen(strbuf_);
	    bufpos_ = 0;
	 }
	 result = write(fd_, strbuf_ + bufpos_, buflen_ - bufpos_);
      }
      if (result < 0)
      {
	 if (errno == EAGAIN)
	 {
	    newcondbits |= UNIXpollManager::FD_Writeable;
	 }
	 else
	 {
	    condbits |= UNIXpollManager::FD_Error;
	 }
      }
      else
      {
	 cerr << "MyEvent(" << fd_ << ") done!\n";
	 close(fd_);
      }
   }
   else if (condbits & ~UNIXpollManager::FD_Writeable)
   {
      int errnocapture = errno;

      cerr << "MyEvent(" << fd_ << ") some weird error happened.\n";
      cerr << "condbits == " << condbits << "\n";
      cerr << "errno == " << errnocapture << "\n";
   }
   if (newcondbits)
   {
      newcondbits |=
	 UNIXpollManager::FD_Error
	 | UNIXpollManager::FD_Closed
	 | UNIXpollManager::FD_Invalid;
      pollman_.registerFDCond(fd_, newcondbits, UNIEventPtrT<PollEvent>(this));
   }
}

int main(int argc, char *argv[])
{
   UNISimpleDispatcher disp;
   UNIXpollManagerImp upm(&disp);
   unsigned int defaultcond = 0;

   defaultcond |=
      UNIXpollManager::FD_Writeable
      | UNIXpollManager::FD_Error
      | UNIXpollManager::FD_Closed
      | UNIXpollManager::FD_Invalid;
   for (int i = 1; i < argc; ++i)
   {
      int fd = open(argv[i], O_WRONLY | O_NONBLOCK);

      if (fd < 0)
      {
	 cerr << "Unable to open " << argv[i]
	      << " for non-blocking writing.\n";
      }
      else
      {
	 upm.registerFDCond(fd, defaultcond,
			    UNIEventPtrT<UNIXpollManager::PollEvent>(new MyEvent(fd, upm)));
      }
   }
   disp.DispatchUntilEmpty();
   return(0);
}
