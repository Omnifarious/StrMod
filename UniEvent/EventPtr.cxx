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
// Revision 1.1  1997/05/13 01:03:25  hopper
// Added new EventPtr classes to maintain reference count on UNIEvent classes.
//

#ifdef __GNUG__
#  pragma implementation "EventPtr.h"
#endif

#include <UniEvent/EventPtr.h>

#ifdef __GNUG__
#  pragma implementation "EventPtrT.h"
#endif

#include <UniEvent/EventPtrT.h>

const UNEVT_ClassIdent UNIEventPtr::identifier(5UL);
