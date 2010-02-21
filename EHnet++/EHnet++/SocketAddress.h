#ifndef _EHNET_SocketAddress_H_  // -*-c++-*-

#ifdef __GNUG__
#pragma interface
#endif

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

/* $Header$ */

 // For a log, see ../ChangeLog
 //
 // Revision 1.4  1996/02/20 01:06:33  hopper
 // Added inline definition for ostream &operator << for SocketAddresses.
 //
 // Revision 1.3  1996/02/12 03:01:55  hopper
 // Added links to my ClassIdent system.
 //
 // Revision 1.2  1996/02/12 00:32:57  hopper
 // Fixed to use the new C++ standard library string class instead of all the
 // 'NetString' silliness.
 //
 // Revision 1.1.1.1  1995/07/23 17:45:30  hopper
 // Imported sources
 //
 // Revision 0.1.0.4  1994/08/12  17:07:26  hopper
 // Changed to use NetString class. The NetString class handles all library
 // dependencies.
 //
 // Revision 0.1.0.3  1994/06/16  03:00:06  hopper
 // Added #pragma interface for GNU stuff.
 //
 // Revision 0.1.0.2  1994/05/08  18:35:08  hopper
 // Changed to work better with Rogue Wave classes.
 //
 // Revision 0.1.0.1  1994/05/08  18:10:25  hopper
 // Head of WinterFire branch. Changed all instances of String with
 // RWCString.
 //
 // Revision 0.1  1994/05/03  03:25:21  hopper
 // Initial revision
 //

 // $Revision$

#include <string>
#include <iosfwd>  // ostream

#define _EHNET_SocketAddress_H_

struct sockaddr;

namespace strmod {
namespace ehnet {

/** \class SocketAddress SocketAddress.h EHnet++/SocketAddress.h
 * C++ class wrapper for struct ::sockaddr from sys/socket.h
 *
 * This is a C++ wrapper for the sockaddr structure that the connect, and bind
 * system calls take.  It includes a virtual 'Copy' method to clone copies when
 * you don't know the actual type of the address.
 */
class SocketAddress {
 public:
   //! An abstract SocketAddress really doesn't have any parameters
   SocketAddress()                                      { }
   //! No member variables, nothing to do
   virtual ~SocketAddress()                             { }

   //! Send a textual representation of the address to the given ostream.
   virtual void PrintOn(::std::ostream &);

   //! Get the sockaddr struct  this object is wrapping.
   virtual ::sockaddr *SockAddr() = 0;
   /** Clone this address, no matter it's actual type
    *
    * This is a non-virtual for compilers that don't support <A HREF="http://cpptips.hyperformix.com/Covariance.html">contravariance</A>
    * in the return types of virtual functions.
   */
   SocketAddress *Copy() const                          { return(MakeCopy()); }
   //! How long is this address in memory?
   virtual int AddressSize() const = 0;
   //! Fetch a string representation of the SocketAddress
   virtual ::std::string AsString() = 0;

 protected:
   //! Clone this address, no matter it's actual type
   virtual SocketAddress *MakeCopy() const = 0;
};

//-----------------------------inline functions--------------------------------

inline ::std::ostream &operator <<(::std::ostream &os, SocketAddress &sa)
{
   sa.PrintOn(os);
   return(os);
}

} // namespace ehnet
} // namespace strmod

#endif
