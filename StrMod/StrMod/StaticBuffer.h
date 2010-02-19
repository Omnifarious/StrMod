#ifndef _STR_StaticBuffer_H_  // -*- mode: c++; c-file-style: "hopper" -*-

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

#include <StrMod/StrChunk.h>
#include <cstddef>

#define _STR_StaticBuffer_H_

namespace strmod {
namespace strmod {

/** \class StaticBuffer StaticBuffer.h StrMod/StaticBuffer.h
 * This class is for pointing at a bunch of data already in memory that will
 * not change or move for the lifetime of the StaticBuffer.  Oftentimes, these
 * will be things like string constants and such.
 */
class StaticBuffer : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   /** Construct a StaticBuffer pointing at the given buffer area.
    *
    * @param buf The area to point at.
    *
    * @param buflen The size of the area to point at.
    */
   inline StaticBuffer(const void *buf, size_t buflen);
   /** Doesn't do a thing because it doesn't own the storage it's pointing
    * at.
    */
   virtual ~StaticBuffer()                              { }

   virtual int AreYouA(const ClassIdent &cid) const {
      return (identifier == cid) || StrChunk::AreYouA(cid);
   }

   virtual unsigned int Length() const                  { return buflen_; }

   //! Get a void pointer to the memory pointed at by this chunk.
   const void *getVoidP()                               { return(buf_); }
   //! Get a possibly more convenient character pointer instead.
   inline const U1Byte *getCharP();

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   inline virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   const void * const buf_;
   const size_t buflen_;
};

//-----------------------------inline functions--------------------------------

inline StaticBuffer::StaticBuffer(const void *buf, size_t buflen)
     : buf_(buf), buflen_(buflen)
{
}

inline const U1Byte *StaticBuffer::getCharP()
{
   return(static_cast<const U1Byte *>(buf_));
}

inline void StaticBuffer::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   call_visitDataBlock(visitor, const_cast<void *>(buf_), buflen_);
}

};  // namespace strmod
};  // namespace strmod

#endif
