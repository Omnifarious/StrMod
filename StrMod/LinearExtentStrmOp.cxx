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

// See ChangLog for log.
// Revision 1.2  1998/08/24 02:08:39  hopper
// Changed to return the value it's supposed to return.
//
// Revision 1.1  1996/06/29 06:53:20  hopper
// New file for implementation of stream operator for class LinearExtent.
//

#include <StrMod/LinearExtent.h>
#include <iostream>

namespace strmod {
namespace strmod {

ostream &operator <<(ostream &os, const LinearExtent &ext)
{
#ifdef __GNUG__
   unsigned long long end;
#else
   unsigned long end;
#endif

   end = ext.Offset();
   end += ext.Length();
   return(os << "[" << ext.Offset() << "--(" << ext.Length()
	     << ")-->" << end << ")");
}

};  // End namespace strmod
};  // End namespace strmod
