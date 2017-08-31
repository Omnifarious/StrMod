/*
 * Copyright 2004-2010 Eric M. Hopper <hopper@omnifarious.org>
 *
 *     This file originally created by Jim Hodapp <jhodapp@iupui.edu>
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

// $URL$
// $Revision$
// $Date$
// $Author$

// See ../ChangeLog for a change log.

#include <StrMod/StrChunkDecorator.h>
//#include <iostream>

namespace strmod {
namespace strmod {

StrChunkDecorator::StrChunkDecorator(const StrChunkPtr &chunkptr)
     : chunkptr_(chunkptr)
{
}

unsigned int StrChunkDecorator::Length() const
{
//   ::std::cerr << " ----> In StrChunkDecorator::Length()\n";
   return chunkptr_->Length();
}

void StrChunkDecorator::acceptVisitor(ChunkVisitor &visitor)
{
   call_visitStrChunk(visitor, chunkptr_);
}

}  // End namespace strmod
}  // End namespace strmod
