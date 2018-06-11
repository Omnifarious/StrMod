#ifndef _STR_StrChunkUDPDecorator_H_  // -*-c++-*-

/*
 * Copyright 2004-2010 Eric M. Hopper <hopper@omnifarious.org>
 *
 *	This file originally created by Jim Hodapp <jhodapp@iupui.edu>
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

// $URL$
// $Revision$
// $Date$
// $Author$

// See ../ChangeLog for a change log.

#include <StrMod/StrChunkDecorator.h>
#include <StrMod/StrChunk.h>
#include <LCore/GenTypes.h>
#include <EHnet++/SocketAddress.h>
#include <EHnet++/InetAddress.h>
#include <string>

#define _STR_StrChunkUDPDecorator_H_

namespace strmod {
namespace strmod {

class StrChunkUDPDecorator : public StrChunkDecorator {
	public:
   StrChunkUDPDecorator(const StrChunkPtr &udpchunkptr, const ehnet::SocketAddress &udpaddr);
   virtual ~StrChunkUDPDecorator();
   //! Returns a SockAddress which will contain the UDP hostname and port
   const ehnet::SocketAddress &SockAddr()            { return sock_addr_; }

 private:
   const ehnet::SocketAddress &sock_addr_;
};

}  // namespace strmod
}  // namespace strmod

#endif
