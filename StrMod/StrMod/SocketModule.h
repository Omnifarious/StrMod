#ifndef _STR_SocketModule_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for a change log.

#ifndef _STR_StreamFDModule_H_
#  include <StrMod/StreamFDModule.h>
#endif

#define _STR_SocketModule_H_

class SocketAddress;
class SockListenModule;
class UNIDispatcher;
class UNIXError;

class SocketModule : public StreamFDModule {
   friend class SockListenModule;  // This is so SockListenModule's can access
                                   // that one protected constructor down there
 public:
   static const STR_ClassIdent identifier;

   //! Create a SocketModule connected to the given address.
   SocketModule(const SocketAddress &addr,
                UNIDispatcher &disp, UNIXpollManager &pollmgr,
		bool blockconnect = true) throw(UNIXError);
   virtual ~SocketModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: Who are we connected to?
   const SocketAddress &GetPeerAddr()                  { return(peer_); }

 protected:
   virtual void writeEOF();

   /** Create a SocketModule using the given fd.
    * Note that ownership of peer is being passed.
    */
   SocketModule(int fd, SocketAddress *peer,
                UNIDispatcher &disp, UNIXpollManager &pollmgr);

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   static int MakeSocket(SocketModule &obj, const SocketAddress &addr,
                         bool blockconnect) throw(UNIXError);

 private:
   SocketAddress &peer_;
};

//-------------------------------inline functions------------------------------

inline int SocketModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamFDModule::AreYouA(cid));
}

#endif
