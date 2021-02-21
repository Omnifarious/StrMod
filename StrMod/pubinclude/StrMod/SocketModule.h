#ifndef _STR_SocketModule_H_  // -*-c++-*-

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

// See ../ChangeLog for a change log.

#include <EHnet++/SocketAddress.h>

#ifndef _STR_StreamFDModule_H_
#  include <StrMod/StreamFDModule.h>
#endif

#define _STR_SocketModule_H_

namespace strmod {
namespace strmod {

class SockListenModule;

class SocketModule : public StreamFDModule
{
   friend class SockListenModule;  // This is so SockListenModule's can access
                                   // that one protected constructor down there
 public:
   //! Create a SocketModule connected to the given address.
   SocketModule(const ehnet::SocketAddress &addr,
                unievent::Dispatcher &disp,
                unievent::UnixEventRegistry &ureg,
                bool blockconnect = true);
   virtual ~SocketModule();

   //! Who are we connected to?
   const ehnet::SocketAddress &GetPeerAddr()           	{ return(peer_); }

   //! What is the local IP address?
   const ehnet::SocketAddress &GetSelfAddr()		{ return(*self_); }

 protected:
   void writeEOF() override;

   //! Create a SocketModule using the given fd.
   /*!
     Note that ownership of peer is being passed.
    */
   SocketModule(int fd, ehnet::SocketAddress *peer,
                unievent::Dispatcher &disp,
                unievent::UnixEventRegistry &ureg);

   static int MakeSocket(SocketModule &obj, const ehnet::SocketAddress &addr,
                         bool blockconnect);

 private:
   ehnet::SocketAddress &peer_;
   ehnet::SocketAddress *self_;

   void setSelfAddr(int fd);
};

}  // namespace strmod
}  // namespace strmod

#endif
