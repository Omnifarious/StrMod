#ifndef _STR_StrChunkDecorator_H_  // -*-c++-*-

/*
 * Copyright 2004-2010 Eric M. Hopper <hopper@omnifarious.org>
 *
 *     This file originally created by Jim Hodapp <jhodapp@iupui.edu>
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

// $URL$
// $Revision$
// $Date$
// $Author$

// See ../ChangeLog for a change log.

#include <StrMod/StrChunk.h>
#include <StrMod/StrChunkPtr.h>

#define _STR_StrChunkDecorator_H_

namespace strmod {
namespace strmod {

class StrChunkDecorator : public StrChunk {
 public:
   StrChunkDecorator(const StrChunkPtr &chunkptr);

   // virtual void Redecorate() const = 0;
   // Virtual members from StrChunk base class
   unsigned int Length() const override;
   //! Accept a ChunkVisitor, and maybe lead it through your children.
   void acceptVisitor(ChunkVisitor &visitor) override;

 private:
   StrChunkPtr chunkptr_;
};

}  // End namespace strmod
}  // End namespace strmod

#endif
