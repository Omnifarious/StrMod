#ifndef _STR_SocketModule_H_  // -*-c++-*-

/*
 * Copyright 1991-2002 Eric M. Hopper <hopper@omnifarious.org>
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

#ifdef __GNUG__
#  pragma interface
#endif

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
   static const STR_ClassIdent identifier;

   //! Create a SocketModule connected to the given address.
   SocketModule(const ehnet::SocketAddress &addr,
                unievent::Dispatcher &disp,
                unievent::UnixEventRegistry &ureg,
                bool blockconnect = true) throw(unievent::UNIXError);
   virtual ~SocketModule();

   inline virtual int AreYouA(const lcore::ClassIdent &cid) const;

   //: Who are we connected to?
   const ehnet::SocketAddress &GetPeerAddr()           { return(peer_); }

   //: What is the local IP address?
   const ehnet::SocketAddress &GetSelfAddr()           { return(*self_); }

 protected:
   virtual void writeEOF();

   /** Create a SocketModule using the given fd.
    * Note that ownership of peer is being passed.
    */
   SocketModule(int fd, ehnet::SocketAddress *peer,
                unievent::Dispatcher &disp,
                unievent::UnixEventRegistry &ureg);

   virtual const lcore::ClassIdent *i_GetIdent() const  { return &identifier; }

   static int MakeSocket(SocketModule &obj, const ehnet::SocketAddress &addr,
                         bool blockconnect) throw(unievent::UNIXError);

 private:
   ehnet::SocketAddress &peer_;
   ehnet::SocketAddress *self_;

   void setSelfAddr(int fd);
};

//-------------------------------inline functions------------------------------

inline int SocketModule::AreYouA(const lcore::ClassIdent &cid) const
{
   return((identifier == cid) || StreamFDModule::AreYouA(cid));
}

};  // namespace strmod
};  // namespace strmod

#endif
