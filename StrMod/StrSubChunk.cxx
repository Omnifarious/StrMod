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
// Revision 1.2  1999/01/17 18:32:05  hopper
// Changed to automatically make a StrSubChunk of a StrSubChunk into just a
// StrSubChunk.
//
// Revision 1.1  1996/06/29 06:57:44  hopper
// New class StrSubChunk used to make a part of an existing chunk look like
// a full chunk to the outside world.
//

#ifdef __GNUG__
#  pragma implementation "StrSubChunk.h"
#endif

#include "StrMod/StrSubChunk.h"
#include "StrMod/StrChunkPtrT.h"
#include "StrMod/StrChunkPtr.h"
#include "StrMod/ChunkVisitor.h"
#include <cassert>
// #include <iostream.h>

const STR_ClassIdent StrSubChunk::identifier(18UL);

StrSubChunk::StrSubChunk(const StrChunkPtr &chunk, const LinearExtent &extent)
     : subchunk_(chunk), subext_(extent)
{
   assert(subchunk_);

   if (subchunk_->AreYouA(identifier))
   {
      StrChunkPtrT<StrSubChunk> subc
	 = static_cast<StrSubChunk *>(subchunk_.GetPtr());

      subext_ = subc->subext_.SubExtent(subext_);
      subchunk_ = subc->subchunk_;
   }
   else
   {
      unsigned int sublen = subchunk_->Length();

      if ((subext_.Offset() + subext_.Length()) > sublen) {
	 LinearExtent chunkext(0, sublen);

//      cerr << "chunkext == " << chunkext
//	   << " && subext_ == " << subext_ << "\n";
	 subext_.SubExtent_eq(chunkext);
//      cerr << "subext_ == " << subext_ << "\n";
      }
   }
}

void StrSubChunk::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   call_visitStrChunk(visitor, subchunk_, subext_);
}
