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

const STR_ClassIdent SocketModule::identifier(12UL);

// MakeSocket sets makesock_errno.
SocketModule::SocketModule(const SocketAddress &addr,
                           UNIDispatcher &disp, UNIXpollManager &pollmgr,
			   bool hangdelete, bool blockconnect)
     : StreamFDModule(MakeSocket(*this, addr, blockconnect), disp, pollmgr,
		      StreamFDModule::CheckBoth,  hangdelete),
       peer(*(addr.Copy()))
{
   if (makesock_errno != 0)
   {
      setErrorIn(ErrFatal, makesock_errno);
   }
   setMaxChunkSize(64U * 1024U);
}

SocketModule::~SocketModule()  // This might be changed later to add
{                              // a shutdown message sent to the
   delete &peer;               // socket on the other side of the
}                              // connection.

SocketModule::SocketModule(int fd, SocketAddress *pr,
                           UNIDispatcher &disp, UNIXpollManager &pollmgr)
     : StreamFDModule(fd, disp, pollmgr, StreamFDModule::CheckBoth, true),
       peer(*pr),
       makesock_errno(0)
{
   setMaxChunkSize(64U * 1024U);
}

bool SocketModule::writeEOF()
{
   if (getFD() >= 0)
   {
      if (readEOF())
      {
	 return(StreamFDModule::writeEOF());
      }
      else
      {
//  	 cerr << "Wheee 1\n";
	 shutdown(getFD(), SHUT_WR);
//  	 cerr << "Wheee 2\n";
	 setErrorIn(ErrWrite, EBADF);
      }
   }
   return(false);
}

static inline int setNonBlock(int fd, int &errval)
{
//     cerr << "In setNonBlock(" << fd << ", " << errval << ")\n";
   if (!FDUtil::setNonBlock(fd, errval))
   {
      close(fd);
      fd = -1;
//        cerr << "setNonBlock::errval == " << errval << "\n";
   }

   return(fd);
}

static inline int doConnect(int fd, SocketAddress &peer, int &errval)
{
   if (connect(fd, peer.SockAddr(), peer.AddressSize()) < 0)
   {
      if (errno != EINPROGRESS)
      {
	 errval = errno;
	 close(fd);
	 fd = -1;
      }
   }
   return(fd);
}

int SocketModule::MakeSocket(SocketModule &obj,
			     const SocketAddress &addr, bool blockconnect)
{
   obj.makesock_errno = 0;
   int fd;
   SocketAddress &peer = *(addr.Copy());

   if ((fd = socket(peer.SockAddr()->sa_family, SOCK_STREAM, PF_UNSPEC)) < 0)
   {
      return(fd);
   }

   if (blockconnect)
   {
      fd = doConnect(fd, peer, obj.makesock_errno);
      if (fd >= 0)
      {
	 fd = setNonBlock(fd, obj.makesock_errno);
      }
   }
   else
   {
      fd = setNonBlock(fd, obj.makesock_errno);
      if (fd >= 0)
      {
	 fd = doConnect(fd, peer, obj.makesock_errno);
      }
   }

   return(fd);
}
