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

#ifdef __GNUG__
#  pragma implementation "BufferChunk.h"
#endif

#include "StrMod/BufferChunk.h"

#include "StrMod/GroupVector.h"
#include <iostream>
#include <cassert>

const STR_ClassIdent BufferChunk::identifier(24UL);

U1Byte BufferChunk::junk_;

void BufferChunk::FillGroupVec(GroupVector &vec, unsigned int &start_index)
{
   if (buflen_ > 0)
   {
      vec.SetVec(start_index++, buf_, buflen_);
   }
}

void BufferChunk::FillGroupVec(const LinearExtent &extent,
			       GroupVector &vec, unsigned int &start_index)
{
   LinearExtent me(0, buflen_);
   me = me.intersection(extent);
   if (me.Length() > 0)
   {
      assert((extent.Length() > 0) && (extent.Offset() < buflen_));
      assert((me.Offset() + me.Length()) <= buflen_);
      U1Byte *tmp = getCharP();
      vec.SetVec(start_index++, tmp + me.Offset(), me.Length());
   }
}

void BufferChunk::printState(ostream &os) const
{
   os << "BufferChunk(buf_ == " << buf_ << ", buflen_ == " << buflen_ << ")";
}
