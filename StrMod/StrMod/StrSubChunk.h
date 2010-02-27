#ifndef _STR_StrSubChunk_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
//
// Revision 1.1  1996/06/29 06:57:50  hopper
// New class StrSubChunk used to make a part of an existing chunk look like
// a full chunk to the outside world.
//

#include <StrMod/StrChunk.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/LinearExtent.h>

#define _STR_StrSubChunk_H_

namespace strmod {
namespace strmod {

/** \class StrSubChunk StrSubChunk.h StrMod/StrSubChunk.h
 * A StrChunk that consists of some part of another StrChunk.
 *
 * This allows you to clip out a portion of another StrChunk without altering
 * the contents of the other StrChunk.
 */
class StrSubChunk : public StrChunk
{
 public:
   StrSubChunk(const StrChunkPtr &chunk, const LinearExtent &extent);
   virtual ~StrSubChunk()                              { }

   inline virtual unsigned int Length() const;

 protected:
   //: Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   StrChunkPtr subchunk_;
   LinearExtent subext_;
};

//-----------------------------inline functions--------------------------------

inline unsigned int StrSubChunk::Length() const
{
   return(subext_.Length());
}

}  // namespace strmod
}  // namespace strmod

#endif
