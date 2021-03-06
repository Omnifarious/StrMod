#ifndef _STR_EOFStrChunk_H_  // -*-c++-*-

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


/* $Header$ */

// For a log, see ../ChangeLog

// $Revision$

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif
#include <memory>

#define _STR_EOFStrChunk_H_

namespace strmod {
namespace strmod {

/** \class EOFStrChunk EOFStrChunk.h StrMod/EOFStrChunk.h
 * \brief A special 'zero length' chunk that indicates a stream EOF.
 *
 * This is mostly for a StreamFDModule to use.  It has an option for generating
 * them when an EOF is read, and if this chunk is written to a StreamFDModule it
 * invokes a special EOF handling virtual function instead of physically writing
 * the chunk to the file descriptor.
 */
class EOFStrChunk : public StrChunk
{
 public:
   EOFStrChunk() = default;
   inline virtual ~EOFStrChunk() = default;

   unsigned int Length() const override                 { return(0); }

 protected:
   void acceptVisitor(ChunkVisitor &) override
   {
   }
};

typedef ::std::shared_ptr<EOFStrChunk> EOFStrChunkPtr;

//-----------------------------inlune functions--------------------------------

}  // namespace strmod
}  // namespace strmod

#endif
