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

// For a log, see Changelog
//
// Revision 1.1  1996/09/02 23:26:41  hopper
// Added PassThrough class to use when you needed a StreamProcessor that
// did nothing except buffer one Chunk worth of data.
//

#ifdef __GNUG__
#  pragma implementation "PassThrough.h"
#endif

#include <StrMod/PassThrough.h>

const STR_ClassIdent PassThrough::identifier(28UL);
