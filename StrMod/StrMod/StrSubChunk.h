#ifndef _STR_StrSubChunk_H_  // -*-c++-*-

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

class StrSubChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   StrSubChunk(const StrChunkPtr &chunk, const LinearExtent &extent);
   virtual ~StrSubChunk()                              { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual unsigned int Length() const;
   inline virtual unsigned int NumSubGroups() const;
   inline virtual unsigned int NumSubGroups(const LinearExtent &extent) const;
   inline virtual void FillGroupVec(GroupVector &vec,
				    unsigned int &start_index);
   inline virtual void FillGroupVec(const LinearExtent &extent,
				    GroupVector &vec,
				    unsigned int &start_index);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual void i_DropUnused(const LinearExtent &usedextent,
				    KeepDir keepdir);

   //: Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   StrChunkPtr subchunk_;
   LinearExtent subext_;
};

//-----------------------------inline functions--------------------------------

inline int StrSubChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline unsigned int StrSubChunk::Length() const
{
   return(subext_.Length());
}

inline unsigned int StrSubChunk::NumSubGroups() const
{
   return(subchunk_->NumSubGroups(subext_));
}

inline unsigned int StrSubChunk::NumSubGroups(const LinearExtent &extent) const
{
   return(subchunk_->NumSubGroups(subext_.SubExtent(extent)));
}

inline void StrSubChunk::i_DropUnused(const LinearExtent &usedextent,
				      KeepDir keepdir)
{
   subchunk_->DropUnused(subext_.SubExtent(usedextent), keepdir);
}

inline void StrSubChunk::FillGroupVec(GroupVector &vec,
				      unsigned int &start_index)
{
   subchunk_->FillGroupVec(subext_, vec, start_index);
}

inline void StrSubChunk::FillGroupVec(const LinearExtent &extent,
				      GroupVector &vec,
				      unsigned int &start_index)
{
   subchunk_->FillGroupVec(subext_.SubExtent(extent), vec, start_index);
}

#endif
