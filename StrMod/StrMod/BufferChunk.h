#ifndef _STR_BufferChunk_H_  // -*-c++-*-

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

#include <StrMod/ChunkVisitor.h>
#include <StrMod/StrChunk.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/LinearExtent.h>
#include <LCore/GenTypes.h>
#include <LCore/Debugable.h>
#include <new>

#define _STR_BufferChunk_H_

//: This is an abstract base class for StrChunks that are really just bags of
//: bytes.
class BufferChunk : public StrChunk, virtual public Debugable {
 public:
   class Factory;
   static const STR_ClassIdent identifier;

   //: Initialize the variables maintained by the derived classes as a
   //: convenience.
   BufferChunk() : buf_(0), buflen_(0)                  { }
   //: Doesn't delete any storage whatsoever.
   // <p>Since this is an abstract class that doesn't ever actually allocate
   // any storage, it's left to the derived classes to do the actual
   // deletion.</p>
   virtual ~BufferChunk()                               { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: See class Debugable.
   inline virtual bool invariant() const;

   //: See class Debugable.
   virtual void printState(ostream &os) const;

   //: See the base class StrChunk.
   virtual unsigned int Length() const                  { return(buflen_); }
   //: See the base class StrChunk.  Returns 1 if Length() is > 0.
   inline virtual unsigned int NumSubGroups() const;
   //: See the base class StrChunk.  Returns 1 or 0, if extent contains more
   //: than 0 bytes.
   inline virtual unsigned int NumSubGroups(const LinearExtent &extent) const;
   //: See the base class StrChunk.
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   //: See the base class StrChunk.
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   //: Returns a reference to the byte at index bnum.
   // If bnum is out of range, the behavior is undefined.
   inline U1Byte &operator [](unsigned int bnum);
   //: Gets a void pointer to at least Length() bytes of data.
   // <p>This guarantees that you can read or write to any byte in the valid
   // range using this pointer as long as you make no calls to Resize or
   // i_DropUNused in between accesses.</p>
   inline void *getVoidP();
   //: Gets a <code>U1Byte</code> pointer to at least <code>Length()</code>
   //: bytes of data.
   // <p>This guarantees that you can read or write to any byte in the valid
   // range using this pointer as long as you make no calls to Resize or
   // i_DropUNused in between accesses.</p>
   inline U1Byte *getCharP();

   //: Change the size of the chunk to newsize.
   // Throws bad_alloc if the allocation fails, just like operator new.
   virtual void resize(unsigned int newsize) throw(bad_alloc) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //: See the base class StrChunk.
   virtual void i_DropUnused(const LinearExtent &usedextent,
			     KeepDir keepdir) = 0;

   //: Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 protected:
   //: <code>buf_</code> is expected to be maintained by the derived class.
   // <p>This exist so that the above inline functions are truly inline.  It's
   // only allowed to change in the constructor, or in respone to the Resize,
   // or i_DropUnused methods being called.</p>
   // <p><code>buf_</code> is required to be non-null when
   // <code>buflen_</code> is greater than 0.</p>
   void *buf_;
   //: <code>buflen_</code> is expected to be maintained by the derived class.
   // <p>This exist so that the above inline functions are truly inline.  It's
   // only allowed to change in the constructor, or in respone to the Resize,
   // or i_DropUnused methods being called.</p>
   unsigned int buflen_;

 private:
   static U1Byte junk_;

   // Left undefined on purpose.
   void operator =(const BufferChunk &b);
   BufferChunk(const BufferChunk &b);
};

//-----------------------------inline functions--------------------------------

inline int BufferChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline bool BufferChunk::invariant() const
{
   return((buflen_ == 0) || (buf_ != 0));
}

inline unsigned int BufferChunk::NumSubGroups() const
{
   return((buflen_ > 0) ? 1 : 0);
}

inline unsigned int BufferChunk::NumSubGroups(const LinearExtent &extent) const
{
   return(((extent.Length() > 0) && (extent.Offset() < buflen_)) ? 1 : 0);
}

inline U1Byte &BufferChunk::operator [](unsigned int bnum)
{
   return((bnum < buflen_) ? static_cast<U1Byte *>(buf_)[bnum] : junk_);
}

inline void *BufferChunk::getVoidP()
{
   return((buflen_ > 0) ? buf_ : &junk_);
}

inline U1Byte *BufferChunk::getCharP()
{
   return((buflen_ > 0) ? static_cast<U1Byte *>(buf_) : &junk_);
}

#endif
