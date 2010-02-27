/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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
#include <iostream>
#include <cassert>

namespace strmod {
namespace strmod {

lcore::U1Byte BufferChunk::junk_;

void BufferChunk::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   call_visitDataBlock(visitor, getVoidP(), buflen_);
}

void BufferChunk::printState(std::ostream &os) const
{
   os << "BufferChunk(buf_ == " << buf_ << ", buflen_ == " << buflen_ << ")";
}

};  // End namespace strmod
};  // End namespace strmod
