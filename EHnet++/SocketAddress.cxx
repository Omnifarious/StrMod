/* $Header$ */

 // $Log$
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
#  include <EHnet++/SocketAddress.h>
#else
#  include "sockaddr.h"
#endif

#include <iostream.h>

void SocketAddress::PrintOn(ostream &os)
{
   os << AsString();
}
