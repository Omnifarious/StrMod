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

// For a log, see ChangeLog
//
// Revision 1.1  1996/02/12 03:01:43  hopper
// Added links to my ClassIdent system.
//

#ifdef __GNUG__
#  pragma implementation "NET_ClassIdent.h"
#endif

namespace strmod {

/** Some classes to handle network addresses, should be overhauled
 *
 * The only notable classes in here are SocketAddress and InetAddress.
 * SocketAddress represents a generic network address that can be fed in as the
 * argument to a 'connect' or 'bind' system call.  InetAddress represents an
 * IPV4 address.
 *
 * If these were really well done, I'd have IPV4Address, IPV4TCPAddress, and
 * IPV4tUDPAddress.  Then IPV4Address wouldn't have a port number, and the other
 * two would.  But, as mention previously, these classes desparately need
 * overhauling, not the least because they use DNS synchronously and can
 * mysteriously halt your program waiting for DNS queries to resolve.
 *
 */
namespace ehnet {

const NET_ClassIdent NET_ClassIdent::identifier(0UL);

} // end namespace ehnet
} // end namespace lcore

// SocketAddress		 1UL
// InetAddress		 2UL
