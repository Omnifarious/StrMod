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
// Revision 1.2  1998/05/01 11:54:33  hopper
// Made various changes so that the UNIXpollManager could automatically
// register itself to be run when the event queue was empty.
// Also, changes to use bool_val and bool_cst instead of bool so that we
// can move to a paltform that doesn't support bool more easily.
//
// Revision 1.1  1998/04/29 01:57:04  hopper
// First cut at making something that can dispatch UNIX system events.
//

#ifdef __GNUG__
#  pragma implementation "UNIXpollManager.h"
#endif

#include "UniEvent/UNIXpollManager.h"

const UNEVT_ClassIdent UNIXpollManager::identifier(7UL);
const UNEVT_ClassIdent UNIXpollManager::PollEvent::identifier(8UL);
