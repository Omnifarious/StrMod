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

// For a log, see ChangeLog
//
// Revision 1.1  1996/02/12 03:01:43  hopper
// Added links to my ClassIdent system.
//

#ifdef __GNUG__
#  pragma implementation "NET_ClassIdent.h"
#endif

#include "EHnet++/NET_ClassIdent.h"

namespace strmod {
namespace ehnet {

const NET_ClassIdent NET_ClassIdent::identifier(0UL);

} // end namespace ehnet
} // end namespace lcore

// SocketAddress		 1UL
// InetAddress		 2UL
