#ifndef _STR_GroupChunk_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.
// Revision 1.1  1996/06/29 06:51:30  hopper
// New class GroupChunk to hold a group of chunks and make them appear to
// be one chunk.
//

#include <StrMod/StrChunk.h>
#include <deque>

#define _STR_GroupChunk_H_

namespace strmod {
namespace strmod {

class StrChunkPtr;

class GroupChunk : public StrChunk {
   typedef std::deque<StrChunk *> ChunkList;
 public:
   static const STR_ClassIdent identifier;

   GroupChunk();
   virtual ~GroupChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                 { return(totalsize_); }

   void push_back(const StrChunkPtr &chnk);
   void push_front(const StrChunkPtr &chnk);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   //: Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   ChunkList chnklist_;
   unsigned int totalsize_;

   GroupChunk(const GroupChunk &);
   void operator =(const GroupChunk &);
};

//-----------------------------inline functions--------------------------------

inline int GroupChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

}  // namespace strmod
}  // namespace strmod

#endif
