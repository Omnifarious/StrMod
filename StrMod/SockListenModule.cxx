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
#  pragma implementation "SockListenModule.h"
#endif

#include "StrMod/SockListenModule.h"

#ifndef _STR_SocketModule_H_
#  include "StrMod/SocketModule.h"
#endif

#include <EHnet++/SocketAddress.h>
#include <EHnet++/InetAddress.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <unistd.h>
#include <netinet/in.h>
#include <cassert>
#include <new>
#include "config.h"
#include "sockdecl.h"
#include <iostream>

namespace strmod {
namespace strmod {

const STR_ClassIdent SockListenModule::identifier(13UL);
const STR_ClassIdent SockListenModule::SLPlug::identifier(14UL);
const STR_ClassIdent SocketModuleChunk::identifier(15UL);

//: A parent class for the three sub event classes.
// <p>The sub event classes don't do anything except call parent class
// protected functions.  The only reason they exist is to avoid having
// a switch statement in the parent.</p>
class SockListenModule::FDPollEv : public unievent::UNIXpollManager::PollEvent {
 public:
   inline FDPollEv(SockListenModule &parent);
   virtual ~FDPollEv()                                 { }

   virtual void triggerEvent(Dispatcher *dispatcher = 0) = 0;

   inline void parentGone()                            { hasparent_ = false; }

 protected:
   inline void triggerRead();
   inline void triggerError();

 private:
   bool hasparent_;
   SockListenModule &parent_;
};

inline SockListenModule::FDPollEv::FDPollEv(SockListenModule &parent)
     : hasparent_(true), parent_(parent)
{
}

inline void SockListenModule::FDPollEv::triggerRead()
{
   // cerr << "In triggerRead\n";
   if (hasparent_)
   {
      unsigned int condbits = getCondBits();
      setCondBits(0);
      parent_.eventRead(condbits);
   }
}

inline void SockListenModule::FDPollEv::triggerError()
{
   // cerr << "In triggerError\n";
   if (hasparent_)
   {
      unsigned int condbits = getCondBits();
      setCondBits(0);
      parent_.eventError(condbits);
   }
}

//: This is one of the three helper classes for SockListenModule::FDPollEv
class SockListenModule::FDPollRdEv : public SockListenModule::FDPollEv {
 public:
   inline FDPollRdEv(SockListenModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollRdEv()                                          { }

   virtual void triggerEvent(Dispatcher *dispatcher = 0)  { triggerRead(); }
};

//: This is one of the three helper classes for SockListenModule::FDPollEv
class SockListenModule::FDPollErEv : public SockListenModule::FDPollEv {
 public:
   inline FDPollErEv(SockListenModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollErEv()                                          { }

   virtual void triggerEvent(Dispatcher *dispatcher = 0)  { triggerError(); }
};

SockListenModule::SockListenModule(const SocketAddress &bind_addr,
                                   unievent::Dispatcher &disp,
				   unievent::UNIXpollManager &pmgr,
				   int qlen)
      : sockfd_(-1), has_error_(false), lplug_(*this),
        plug_pulled_(false), checking_read_(false), newmodule_(0),
	myaddr_(*(bind_addr.Copy())),
	disp_(disp), pmgr_(pmgr), readevptr_(0), errorevptr_(0)
{
   using unievent::UNIXError;
   sockfd_ = socket(myaddr_.SockAddr()->sa_family, SOCK_STREAM, PF_UNSPEC);
   if (sockfd_ < 0)
   {
      setError(UNIXError("socket",
                         LCoreError("Creating listening socket",
                                    LCORE_GET_COMPILERINFO())));
      return;
   }
   {
      int temp = fcntl(sockfd_, F_GETFL, 0);

      if (temp < 0)
      {
         setError(UNIXError("fcntl",
                            LCoreError("Setting non-blocking mode",
                                       LCORE_GET_COMPILERINFO())));
	 close(sockfd_);
	 sockfd_ = -1;
	 return;
      }
      temp &= ~O_NDELAY;
      if (fcntl(sockfd_, F_SETFL, temp | O_NONBLOCK) < 0)
      {
         setError(UNIXError("fcntl",
                            LCoreError("Setting non-blocking mode",
                                       LCORE_GET_COMPILERINFO())));
	 close(sockfd_);
	 sockfd_ = -1;
	 return;
      }
   }
   if (bind(sockfd_, myaddr_.SockAddr(), myaddr_.AddressSize()) < 0) {
      setError(UNIXError("fcntl",
                         LCoreError("Binding listening socket",
                                    LCORE_GET_COMPILERINFO())));
      close(sockfd_);
      sockfd_ = -1;
      return;
   }
   if (listen(sockfd_, qlen) < 0)
   {
      setError(UNIXError("listen",
                         LCoreError("Listening on listening socket",
                                    LCORE_GET_COMPILERINFO())));
      close(sockfd_);
      sockfd_ = -1;
      return;
   }
   readevptr_ = new FDPollRdEv(*this);
   std::cerr << "readevptr_ == " << readevptr_ << "\n";
   readev_ = readevptr_;
   errorevptr_ = new FDPollErEv(*this);
   std::cerr << "errorevptr_ == " << errorevptr_ << "\n";
   errorev_ = errorevptr_;
   checking_read_ = true;
   pmgr_.registerFDCond(sockfd_,
			UNIXpollManager::FD_Error
			| UNIXpollManager::FD_Closed
			| UNIXpollManager::FD_Invalid,
			errorev_);
   pmgr_.registerFDCond(sockfd_, UNIXpollManager::FD_Readable, readev_);
}

SockListenModule::~SockListenModule()
{
   if (sockfd_ >= 0)
   {
      close(sockfd_);
      pmgr_.freeFD(sockfd_);
   }
   delete &myaddr_;
   if (readevptr_)
   {
      readevptr_->parentGone();
   }
   if (errorevptr_)
   {
      errorevptr_->parentGone();
   }
}

void SockListenModule::eventRead(unsigned int condbits)
{
   using unievent::UNIXError;
   checking_read_ = false;
   if (!(condbits & UNIXpollManager::FD_Readable))
   {
      setError(UNIXError("<none>", 0,
                         LCoreError("Got fd condition other than readable",
                                    LCORE_GET_COMPILERINFO())));
   }
   else if (newmodule_ == 0)
   {
      doAccept();
   }
}

void SockListenModule::eventError(unsigned int condbits)
{
   using unievent::UNIXError;
   setError(UNIXError("<none>", 0,
                      LCoreError("Got error condition",
                                 LCORE_GET_COMPILERINFO())));
}

void SockListenModule::doAccept()
{
   using unievent::UNIXError;
   assert(newmodule_ == 0);

   if (newmodule_ != 0)
   {
      setReadableFlagFor(&lplug_, true);
      return;
   }

   unsigned char addrbuf[8192];
   struct sockaddr *saddr = reinterpret_cast<struct sockaddr *>(addrbuf);
   size_t length = sizeof(addrbuf);
#ifdef ACCEPT_NEEDS_INT
   int stupid_compatibility_trick = length;
   int tempfd = accept(sockfd_, saddr, &stupid_compatibility_trick);
   length = stupid_compatibility_trick;
#else
   int tempfd = accept(sockfd_, saddr, &length);
#endif

   if (tempfd < 0)
   {
      if (errno != EAGAIN)
      {
         setError(UNIXError("accept",
                            LCoreError("Error accepting connection",
                                       LCORE_GET_COMPILERINFO())));
      }
      else if (!checking_read_)
      {
	 pmgr_.registerFDCond(sockfd_,
			      UNIXpollManager::FD_Readable,
			      readev_);
	 checking_read_ = true;
      }
   }
   else
   {
      if (saddr->sa_family != AF_INET)
      {
	 close(tempfd);
         setError(UNIXError("<none>", 0,
                            LCoreError("Got connection from non-AF_INET peer",
                                       LCORE_GET_COMPILERINFO())));
      }
      else
      {
	 {
	    int temp = fcntl(tempfd, F_GETFL, 0);

	    if (temp < 0)
	    {
               setError(UNIXError("fcntl",
                                  LCoreError("Setting accepted connection non-blocking",
                                             LCORE_GET_COMPILERINFO())));
	       close(tempfd);
	       tempfd = -1;
	    }
	    else
	    {
	       temp &= ~O_NDELAY;
	       if (fcntl(tempfd, F_SETFL, temp | O_NONBLOCK) < 0)
	       {
                  setError(UNIXError("fcntl",
                                     LCoreError("Setting accepted connection non-blocking",
                                                LCORE_GET_COMPILERINFO())));
		  close(tempfd);
		  tempfd = -1;
		  return;
	       }
	    }
	 }

	 if (tempfd >= 0)
	 {
	    sockaddr_in *sinad
	       = reinterpret_cast<struct sockaddr_in *>(saddr);
	    InetAddress *addr = new InetAddress(*sinad);

	    newmodule_ = makeSocketModule(tempfd, addr, disp_, pmgr_);
	 }
      }
   }
   if (newmodule_ != 0)
   {
      setReadableFlagFor(&lplug_, true);
   }
   else
   {
      setReadableFlagFor(&lplug_, false);
   }
}

void SockListenModule::clearError() throw()
{
   using unievent::UNIXError;
   if (has_error_)
   {
      (reinterpret_cast<UNIXError *>(errorstore_))->~UNIXError();
      has_error_ = false;
   }
}

const unievent::UNIXError &SockListenModule::getError() const throw()
{
   using unievent::UNIXError;
   const void *raw = errorstore_;
   return *reinterpret_cast<const UNIXError *>(raw);
}

void SockListenModule::setError(const unievent::UNIXError &err) throw()
{
   using unievent::UNIXError;
   void *raw = errorstore_;
   UNIXError *store = reinterpret_cast<UNIXError *>(raw);
   if (has_error_)
   {
      store->~UNIXError();
   }
   else
   {
      has_error_ = true;
   }
   new(raw) UNIXError(err);
}

SocketModule *SockListenModule::getNewModule()
{
   SocketModule *retval = newmodule_;

   if (newmodule_ != 0)
   {
      newmodule_ = 0;
      doAccept();
   }
   return(retval);
}

const SocketModuleChunkPtr SockListenModule::SLPlug::getConnection()
{
   return(new SocketModuleChunk(getParent().getNewModule()));
}

const StrChunkPtr SockListenModule::SLPlug::i_Read()
{
   return(getConnection());
}

void SockListenModule::SLPlug::i_Write(const StrChunkPtr &chunk)
{
   assert(false);
}

};  // End namespace strmod
};  // End namespace strmod
