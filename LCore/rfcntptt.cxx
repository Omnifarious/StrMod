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
// Revision 1.2  1997/05/12 16:26:27  hopper
// Fixed #include directives so auto-dependency generator would work better.
//
// Revision 1.1  1997/05/12 14:32:40  hopper
// Added new RefCountPtr class, and RefCountPtrT class to aid in using
// the ReferenceCounting mixin class.
//

#ifdef __GNUG__
#  pragma implementation "RefCountPtrT.h"
#endif

#include "LCore/RefCountPtrT.h"
#include "LCore/RefCountPtr.h"
