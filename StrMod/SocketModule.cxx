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

#ifndef NO_RcsID
static char _SocketModule_CC_rcsID[] =
      "$Id$";
#endif

#ifdef __GNUG__
#  pragma implementation "SocketModule.h"
#endif

#include <EHnet++/SocketAddress.h>
#include "StrMod/SocketModule.h"
#include "StrMod/FDUtil.h"
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "config.h"
#include "sockdecl.h"

#ifndef SHUT_WR
#define SHUT_WR 1
#endif

using strmod::unievent::UNIXError;

const STR_ClassIdent SocketModule::identifier(12UL);

// MakeSocket sets makesock_errno_.
SocketModule::SocketModule(const SocketAddress &addr,
                           strmod::unievent::Dispatcher &disp,
                           strmod::unievent::UNIXpollManager &pollmgr,
                           bool blockconnect)
   throw(UNIXError)
     : StreamFDModule(MakeSocket(*this, addr, blockconnect), disp, pollmgr,
		      StreamFDModule::CheckBoth),
       peer_(*(addr.Copy()))
{
   setMaxChunkSize(64U * 1024U);
}

SocketModule::~SocketModule()  // This might be changed later to add
{                              // a shutdown message sent to the
   delete &peer_;              // socket on the other side of the
}                              // connection.

SocketModule::SocketModule(int fd, SocketAddress *pr,
                           strmod::unievent::Dispatcher &disp,
                           strmod::unievent::UNIXpollManager &pollmgr)
     : StreamFDModule(fd, disp, pollmgr, StreamFDModule::CheckBoth),
       peer_(*pr)
{
   setMaxChunkSize(64U * 1024U);
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
//  	 cerr << "Wheee 1\n";
         if (shutdown(getFD(), SHUT_WR) != 0)
         {
//  	 cerr << "Wheee 2\n";
            setErrorIn(ErrWrite,
                       UNIXError("shutdown",
                                 LCoreError(LCORE_GET_COMPILERINFO())));
         }
         else
         {
            setErrorIn(ErrWrite,
                       UNIXError("shutdown",
                                 LCoreError(LCORE_GET_COMPILERINFO()), true));
         }
      }
   }
}

static inline void setNonBlock(int fd) throw(UNIXError)
{
   int errval = 0;
//     cerr << "In setNonBlock(" << fd << ", " << errval << ")\n";
   if (!FDUtil::setNonBlock(fd, errval))
   {
      throw UNIXError("FDUtil::setNonBlock", errval,
                      LCoreError(LCORE_GET_COMPILERINFO()));
//        cerr << "setNonBlock::errval == " << errval << "\n";
   }
}

static inline void doConnect(int fd, SocketAddress &peer) throw(UNIXError)
{
   if (connect(fd, peer.SockAddr(), peer.AddressSize()) < 0)
   {
      if (errno != EINPROGRESS)
      {
         throw UNIXError("connect", LCoreError(LCORE_GET_COMPILERINFO()));
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
         throw UNIXError("socket", LCoreError(LCORE_GET_COMPILERINFO()));
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
