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

// For log, see ChangeLog
//
// $Revision$
//
// Revision 1.2  1998/06/02 01:08:15  hopper
// Changed a static_cast to a more correct reinterpret_cast.
//
// Revision 1.1  1996/02/12 00:32:35  hopper
// Fixed to use the new C++ standard library string class instead of all the
// 'NetString' silliness.
//

// $Revision$

#include <EHnet++/InetAddress.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <string>

/* My UNIX (UnixWare) has no declaration for gethostname in any header
   file anywhere.  I should get GNU configure to check for this, and
   only use the declaration below conditionally, but that'll have to
   wait. */
extern "C" int gethostname(char *name, unsigned int namelen);

namespace strmod {
namespace ehnet {

bool InetAddress::NameToIaddr(const char *name_addr, U4Byte &num)
{
   ::hostent *hostinfo;
   char *temp_name = const_cast<char *>(name_addr);

   hostinfo = ::gethostbyname(temp_name);
   if (hostinfo == 0 || hostinfo->h_addrtype != AF_INET) {
      return(false);
   } else {
      num = ((struct in_addr *)(hostinfo->h_addr_list[0]))->s_addr;
      return(true);
   }
}

::std::string InetAddress::IaddrToName(const sockaddr_in &inaddr)
{
   ::std::string hostname;
   ::hostent *hostinfo;

   /* If we're dealing with a non-INADDR_ANY address. */
   if (inaddr.sin_addr.s_addr != INADDR_ANY) {
      char *cast_addr;

      /* Do messy casting to type gethostbyaddr likes. */
      cast_addr = const_cast<char *>(
	 reinterpret_cast<const char *>(&inaddr.sin_addr));
      /* Look up the name for the given address.*/
      hostinfo = ::gethostbyaddr(cast_addr, sizeof(inaddr.sin_addr), AF_INET);

      if (hostinfo != 0) {
	 /* Eureka! We found a 'real' hostname for our internet number. */

	 if (inaddr.sin_addr.s_addr != INADDR_ANY) {
	    hostname = hostinfo->h_name;
	 }
      } else {
	 /* Else, no name information was found for this inaddr, so convert
	    the inaddr to the <num>.<num>.<num>.<num> form.  Precedent for
	    doing this has been set by many UNIIX utilities, most notably,
	    traceroute. */

	 unsigned long addr = ntohl(inaddr.sin_addr.s_addr);

	 hostname = ToDec((addr >> 24) & 0xffUL);
	 hostname += '.';
	 hostname += ToDec((addr >> 16) & 0xffUL);
	 hostname += '.';
	 hostname += ToDec((addr >> 8) & 0xffUL);
	 hostname += '.';
	 hostname += ToDec(addr & 0xffUL);
      }
   } else {
      /* INADDR_ANY addresses are used by bind to set up ports on the local
         machine.  Not using INADDR_ANY relegates us to having several
         different ports for several different IPs.  All hosts have AT LEAST
         two IPs, their real internet number, and localhost loopback
         (127.0.0.1). Computers that double as routers may have many more,
         one for each subnet that they 'appear' on. */
      /* This means that the hostname for an INADDR_ANY address is the local
         machine's hostname. */

      char buf[MAXHOSTNAMELEN + 1];

      ::gethostname(buf, MAXHOSTNAMELEN + 1);
      hostinfo = ::gethostbyname(buf);
      hostname = "INADDR_ANY(";
      if (hostinfo != 0) {
	 hostname += hostinfo->h_name;
      } else {
	 hostname += buf;
      }
      hostname += ")";
   }
   return(hostname);
}

} // end namespace ehnet
} // end namespace lcore
