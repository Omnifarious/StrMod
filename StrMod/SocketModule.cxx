/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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
#  pragma implementation "SocketModule.h"
#endif

#include <EHnet++/SocketAddress.h>
#include <EHnet++/InetAddress.h>
#include "StrMod/SocketModule.h"
#include "StrMod/FDUtil.h"
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <unistd.h>
#include "config.h"
#include "sockdecl.h"

#include <iostream>

#ifndef SHUT_WR
#define SHUT_WR 1
#endif

namespace strmod {
namespace strmod {

using unievent::UNIXError;
using ehnet::SocketAddress;
using lcore::LCoreError;

// MakeSocket sets makesock_errno_.
SocketModule::SocketModule(const SocketAddress &addr,
                           unievent::Dispatcher &disp,
                           unievent::UnixEventRegistry &ureg,
                           bool blockconnect)
   throw(UNIXError)
     : StreamFDModule(MakeSocket(*this, addr, blockconnect), disp, ureg,
                      StreamFDModule::CheckBoth),
       peer_(*(addr.Copy())),
       self_(0)
{
   setMaxChunkSize(64U * 1024U);
   setSelfAddr(getFD());
}

SocketModule::~SocketModule()  // This might be changed later to add
{                              // a shutdown message sent to the
   delete &peer_;              // socket on the other side of the
   delete self_;               // connection.
}

SocketModule::SocketModule(int fd, SocketAddress *pr,
                           unievent::Dispatcher &disp,
                           unievent::UnixEventRegistry &ureg)
     : StreamFDModule(fd, disp, ureg, StreamFDModule::CheckBoth),
       peer_(*pr),
       self_(0)
{
   setMaxChunkSize(64U * 1024U);
   setSelfAddr(fd);
}

void SocketModule::writeEOF()
{
   if (getFD() >= 0)
   {
      if (hasErrorIn(ErrRead) && getErrorIn(ErrRead).isEOF())
      {
         StreamFDModule::writeEOF();
      }
      else
      {
//         ::std::cerr << "Wheee 1\n";
         if (shutdown(getFD(), SHUT_WR) != 0)
         {
//            ::std::cerr << "Wheee 2\n";
            const int myerrno = UNIXError::getErrno();
            setErrorIn(ErrWrite,
                       UNIXError("shutdown", myerrno,
                                 LCoreError(LCORE_GET_COMPILERINFO())));
         }
         else
         {
            setErrorIn(ErrWrite,
                       UNIXError("shutdown",
                                 LCoreError(LCORE_GET_COMPILERINFO())));
         }
      }
   }
}

static inline void setNonBlock(int fd) throw(UNIXError)
{
   int errval = 0;
//     ::std::cerr << "In setNonBlock(" << fd << ", " << errval << ")\n";
   if (!FDUtil::setNonBlock(fd, errval))
   {
      throw UNIXError("FDUtil::setNonBlock", errval,
                      LCoreError(LCORE_GET_COMPILERINFO()));
//        ::std::cerr << "setNonBlock::errval == " << errval << "\n";
   }
}

static inline void doConnect(int fd, SocketAddress &peer) throw(UNIXError)
{
   if (connect(fd, peer.SockAddr(), peer.AddressSize()) < 0)
   {
      const int myerrno = UNIXError::getErrno();
      if (myerrno != EINPROGRESS)
      {
         throw UNIXError("connect", myerrno,
                         LCoreError(LCORE_GET_COMPILERINFO()));
      }
   }
}

void SocketModule::setSelfAddr(int fd)
{
   unsigned char addrbuf[8192];
   struct sockaddr *saddr = reinterpret_cast<struct sockaddr *>(addrbuf);
   socklen_t length = sizeof(addrbuf);

   if (getsockname(fd, saddr, &length) == 0)
   {
      if (saddr->sa_family != AF_INET)
      {
//         std::cerr << "Got connection from non-AF_INET peer" << std::endl;
         // Got connection from non-AF_INET peer.
         setErrorIn(ErrWrite,
                    UNIXError("<none>", 0,
                              LCoreError("Got connection from non-AF_INET peer",
                                         LCORE_GET_COMPILERINFO())));
      }
      else
      {
         sockaddr_in *sinad = reinterpret_cast<struct sockaddr_in *>(saddr);
         self_ = new ehnet::InetAddress(*sinad);
         // ::std::cerr << "self_ = " << *(self_) << std::endl;
      }
   }
}

int SocketModule::MakeSocket(SocketModule &obj,
                             const SocketAddress &addr, bool blockconnect)
   throw(UNIXError)
{
   int fd = -1;
   try {
      SocketAddress &peer = *(addr.Copy());

      if ((fd = socket(peer.SockAddr()->sa_family, SOCK_STREAM, PF_UNSPEC)) < 0)
      {
         const int myerrno = UNIXError::getErrno();
         throw UNIXError("socket", myerrno,
                         LCoreError(LCORE_GET_COMPILERINFO()));
      }

      if (blockconnect)
      {
          doConnect(fd, peer);
          setNonBlock(fd);
      }
      else
      {
         setNonBlock(fd);
         doConnect(fd, peer);
      }
   }
   catch(UNIXError e)
   {
      if (fd >= 0)
      {
         close(fd);
         fd = -1;
      }
      throw;
   }
   return(fd);
}

};  // End namespace strmod
};  // End namespace strmod
