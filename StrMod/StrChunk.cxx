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

// See ./ChangeLog for changes.

#ifdef __GNUG__
#  pragma implementation "StrChunk.h"
#endif

#include "StrMod/StrChunk.h"
#include "StrMod/LinearExtent.h"

const STR_ClassIdent StrChunk::identifier(6UL);

unsigned int StrChunk::NumSubGroups() const
{
   return(NumSubGroups(LinearExtent::full_extent));
}

void StrChunk::FillGroupVec(GroupVector &vec, unsigned int &start_index)
{
   FillGroupVec(LinearExtent::full_extent, vec, start_index);
}
