#ifndef _STR_GroupChunk_H_  // -*-c++-*-

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

// See ../ChangeLog for log.
// Revision 1.1  1996/06/29 06:51:30  hopper
// New class GroupChunk to hold a group of chunks and make them appear to
// be one chunk.
//

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif
#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif
#include <deque>

#define _STR_GroupChunk_H_

namespace strmod {
namespace strmod {

/** \class GroupChunk GroupChunk.h StrMod/GroupChunk.h
 * \brief A StrChunk that consists of a concatentation of other StrChunks.
 *
 * GroupChunks are used to concatenate together a bunch of StrChunks without
 * having to move any of their memory.  To the outside, a GroupChunk appears as
 * a single, contiguous StrChunk, but internally it is composed of a list of
 * pointers to it's subsidiary StrChunks.
 *
 * This class plays the role of Composite in the <a href="http://exciton.cs.oberlin.edu/javaresources/DesignPatterns/composite.htm">Composite</a>
 * pattern.
 */
class GroupChunk : public StrChunk
{
   typedef std::deque<StrChunkPtr> ChunkList;
 public:
   //! Construct an empty GroupChunk
   GroupChunk();
   //! Dereferences all direct children.
   virtual ~GroupChunk();

   unsigned int Length() const override                 { return(totalsize_); }

   //! Concatenate a StrChunk to the end of the GroupChunk
   void push_back(const StrChunkPtr &chnk);
   //! Prepend a StrChunk to this GroupChunk
   void push_front(const StrChunkPtr &chnk);

 protected:
   //: Accept a ChunkVisitor, and maybe lead it through your children.
   void acceptVisitor(ChunkVisitor &visitor) override;

 private:
   ChunkList chnklist_;
   unsigned int totalsize_;

   GroupChunk(const GroupChunk &);
   void operator =(const GroupChunk &);
};

}  // namespace strmod
}  // namespace strmod

#endif
