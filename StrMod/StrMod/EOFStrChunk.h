#ifndef _STR_EOFStrChunk_H_  // -*-c++-*-

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

// $Revision$

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif
#ifndef _STR_StrChunkPtrT_H_
#  include <StrMod/StrChunkPtrT.h>
#endif

#define _STR_EOFStrChunk_H_

//: A special 'zero length' chunk that indicates a stream EOF.
// This is mostly for a StreamFDModule to use.  It has an option for
// generating them when an EOF is read, and if this chunk is written to a
// StreamFDModule it invokes a special EOF handling virtual function instead
// of physically writing the chunk to the file descriptor.
class EOFStrChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   EOFStrChunk()                                       { }
   inline virtual ~EOFStrChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                 { return(0); }
   virtual unsigned int NumSubGroups() const           { return(0); }
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
};

typedef StrChunkPtrT<EOFStrChunk> EOFStrChunkPtr;

//-----------------------------inlune functions--------------------------------

inline EOFStrChunk::~EOFStrChunk()
{
}

inline int EOFStrChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline unsigned int
EOFStrChunk::NumSubGroups(const LinearExtent &) const
{
   return(0);
}

inline void EOFStrChunk::FillGroupVec(GroupVector &, unsigned int &)
{
}

inline void EOFStrChunk::FillGroupVec(const LinearExtent &,
					    GroupVector &, unsigned int &)
{
}

inline void EOFStrChunk::i_DropUnused(const LinearExtent &, KeepDir)
{
}

#endif
