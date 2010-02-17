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
//
 // Revision 1.3  1996/02/20 01:05:52  hopper
 // Added some niceties to GNUmakefile.in
 // Changed SocketAddress.cc to use new #include convention.
 //
 // Revision 1.2  1996/02/12 00:32:38  hopper
 // Fixed to use the new C++ standard library string class instead of all the
 // 'NetString' silliness.
 //
 // Revision 1.1.1.1  1995/07/23 17:45:29  hopper
 // Imported sources
 //
 // Revision 0.2  1995/01/06  14:05:29  hopper
 // Merged 0.1.0.5 and 0.1
 //
 // Revision 0.1.0.5  1995/01/06  14:00:13  hopper
 // Put some changes in to make this work under OS/2 again.
 //
 // Revision 0.1.0.4  1994/08/12  17:06:26  hopper
 // Changed to use NetString class. The NetString class handles all library
 // dependencies.
 //
 // Revision 0.1.0.3  1994/07/18  03:30:20  hopper
 // Added a #pragma implementation line so it would work better with gcc 2.6.0
 //
 // Revision 0.1.0.2  1994/05/08  18:32:47  hopper
 // Changed to work better with Rogue Wave classes.
 //
 // Revision 0.1.0.1  1994/05/08  18:12:18  hopper
 // Head of WinterFire branch. Changed all instances of String with
 // RWCString.
 //
 // Revision 0.1  1994/05/03  03:23:38  hopper
 // Initial revision.
 //

 // $Revision$

#ifdef __GNUG__
#  pragma implementation "SocketAddress.h"
#endif

#ifndef OS2
#  include "EHnet++/SocketAddress.h"
#else
#  include "sockaddr.h"
#endif

#include <iostream.h>

void SocketAddress::PrintOn(ostream &os)
{
   os << AsString();
}
