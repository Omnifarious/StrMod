#ifndef _EHNET_InetAddress_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/*
 * Copyright 1999-2010 Eric M. Hopper <hopper@omnifarious.org>
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

 // For log, see ../ChangeLog

 // $Revision$

 // Revision 1.3  1996/02/12 03:01:53  hopper
 // Added links to my ClassIdent system.
 //
 // Revision 1.2  1996/02/12 00:32:55  hopper
 // Fixed to use the new C++ standard library string class instead of all the
 // 'NetString' silliness.
 //
 // Revision 1.1.1.1  1995/07/23 17:45:30  hopper
 // Imported sources
 //
 // Revision 0.2.0.4.1.1  1995/07/23 03:21:38  hopper
 // Added #include <string.h> so it would compile correctly used libg++ 2.7.0.
 // libg++ 2.7.0 is in error.
 //
 // Revision 0.2.0.4  1994/08/12 17:07:12  hopper
 // Changed to use NetString class. The NetString class handles all library
 // dependencies.
 //
 // Revision 0.2.0.3  1994/06/16  02:59:39  hopper
 // Added #pragma interface for GNU stuff.
 //
 // Revision 0.2.0.2  1994/05/08  18:34:52  hopper
 // Changed to work better with Rogue Wave classes.
 //
 // Revision 0.2.0.1  1994/05/08  18:06:21  hopper
 // Head of WinterFire branch. Changed all instances of String with
 // RWCString.
 //
 // Revision 0.2  1994/05/08  16:11:06  hopper
 // Shifted declarations and definitions to work better with new libraries.
 //
 // Revision 0.1  1994/05/03  03:25:21  hopper
 // Initial revision
 //

#ifndef _EHNET_SocketAddress_H_
#   include <EHnet++/SocketAddress.h>
#endif
#include <LCore/GenTypes.h>

#include <string>
#include <sys/types.h>
#include <netinet/in.h>

#define _EHNET_InetAddress_H_

namespace strmod {
namespace ehnet {

/** \class InetAddress InetAddress.h EHnet++/InetAddress.h
 * An IPV4 TCP or UDP address.
 *
 * This class represents an IPV4 TCP or UDP address.  It doesn't represent a
 * straight IPV4 address because it has a port number.
 *
 * This class has the nasty habit of doing blocking DNS lookups at random times.
 * This may cause a several second pause while they're going on.
 */
class InetAddress : public SocketAddress {
 private:
   typedef lcore::U2Byte U2Byte;
   typedef lcore::U4Byte U4Byte;

 public:
   /** Create an InetAddress from a hostname/dotted-quad and port #
    * @param h_name A host name (i.e. omnifarious.omnifarious.org) , or dotted-quad (i.e. 208.42.65.33).
    * @param prt A TCP or UDP port number.
    */
   InetAddress(const ::std::string &h_name, U2Byte prt);
   /** Create an InetAddress pointing an INADDR_ANY and a port #
    * @param port A TCP or UDP port number
    */
   InetAddress(U2Byte port);
   //! Create a copy of another InetAddress
   InetAddress(const InetAddress &b);
   /** Create an InetAddress from the associated sockaddr_in structure
    *
    * Useful for addresses you get back from accept(2) or getpeername(2)
    */
   InetAddress(const ::sockaddr_in &iadr);
   //! No pointers or anything, so not much to destroy
   virtual ~InetAddress()                               { }

   virtual ::sockaddr *SockAddr()         { return (::sockaddr *)(&inaddr); }
   InetAddress *Copy() const              { return (InetAddress *)MakeCopy(); }
   virtual int AddressSize() const        { return sizeof(::sockaddr_in); }
   virtual ::std::string AsString();

   //! Get the host name associated with this address, possibly the result of a reverse DNS lookup
   ::std::string GetHostname() const                    { return(hostname); }
   //! Get the host name associated with this address, possibly the result of a reverse DNS lookup
   ::std::string GetHostname(bool forcelookup);
   //! Get the port number
   U2Byte GetPort() const                               { return port; }
   //! Get IP address
   U4Byte getRawIP() const 		{ return ntohl(inaddr.sin_addr.s_addr); }

   //! Only necessary because of the DNS lookups.
   const InetAddress &operator =(const InetAddress &b);
   /** Copy an InetAddress from a sockaddr_in structure.
    *
    * Useful for addresses you get back from accept(2) or getpeername(2).
    */
   const InetAddress &operator =(const ::sockaddr_in &iadr);

 protected:
   inline virtual SocketAddress *MakeCopy() const;

   //! Mark this address as invalid
   void InvalidateAddress();
   //! Parse out a dotted-quad
   static bool ParseNumeric(const char *numeric_addr, U4Byte &num);
   //! Lookup an IPV4 address from a hostname - returns false on failure.
   static bool NameToIaddr(const char *name_addr, U4Byte &num);
   //! Lookup a hostname from an IPV4 address.
   static ::std::string IaddrToName(const ::sockaddr_in &inaddr);

 private:
   ::std::string hostname;
   U2Byte port;
   ::sockaddr_in inaddr;

   static bool AsciiToQInum(const char *s, int &i,
			    unsigned long &num, bool endsindot);
   static ::std::string ToDec(U2Byte num);
};

//--------------------------------inline functions-----------------------------

inline SocketAddress *InetAddress::MakeCopy() const
{
   return(new InetAddress(*this));
}

} // end namespace ehnet
} // end namespace strmod

#endif
