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

// See ChangeLog for log.
// Revision 1.1  1996/06/29 06:51:22  hopper
// New class GroupChunk to hold a group of chunks and make them appear to
// be one chunk.
//

#ifdef __GNUG__
#  pragma implementation "GroupChunk.h"
#endif

#include <StrMod/GroupChunk.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/StrChunk.h>
#include <StrMod/LinearExtent.h>
// #include <iostream>

namespace strmod {
namespace strmod {

const STR_ClassIdent GroupChunk::identifier(20UL);

// We deal with StrChunk *'s directly because it's more efficient.  We try
// to hide this fact as much as possible from the outside world.

GroupChunk::GroupChunk() : totalsize_(0)
{
}

GroupChunk::~GroupChunk()
{
   ChunkList::iterator i;
   ChunkList::iterator end;

   end = chnklist_.end();
   for (i = chnklist_.begin(); i != end; ++i) {
      StrChunk *ptr = *i;

      *i = 0;
      if (ptr) {
	 ptr->DelReference();
	 if (ptr->NumReferences() <= 0) {
	    delete ptr;
	 }
      }
   }
}

void GroupChunk::push_back(const StrChunkPtr &chnk)
{
   StrChunk *ptr = chnk.GetPtr();

   ptr->AddReference();
   chnklist_.push_back(ptr);
   totalsize_ += ptr->Length();
}

void GroupChunk::push_front(const StrChunkPtr &chnk)
{
   StrChunk *ptr = chnk.GetPtr();

   ptr->AddReference();
   chnklist_.push_front(ptr);
   totalsize_ += ptr->Length();
}

void GroupChunk::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   for (ChunkList::const_iterator i = chnklist_.begin(),
                            listend = chnklist_.end(); i != listend; ++i)
   {
      call_visitStrChunk(visitor, *i);
   }
}

}  // End namespace strmod
}  // End namespace strmod
