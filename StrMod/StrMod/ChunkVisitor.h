#ifndef _STR_ChunkVisitor_H_  // -*-c++-*-

/*
 * Copyright 2000 by Eric M. Hopper <hopper@omnifarious.mn.org>
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

#include <LCore/Protocol.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/LinearExtent.h>
#include <stdexcept>
#include <cstddef>

#define _STR_ChunkVisitor_H_

class StrChunkPtr;
class StrChunk;

class ChunkVisitor : public Protocol {
   friend class StrChunk;
 public:
   //: An exception to allow the visitor to halt the traversal.
   class halt_visitation : public exception {
   };
   static const STR_ClassIdent identifier;

   ChunkVisitor()                                   { }
   virtual ~ChunkVisitor()                          { }

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || Protocol::AreYouA(cid));
   }

   // Derived classes should provide some sort of 'visit' method that
   // takes at least the top level chunk to be visited as an argument.

 protected:
   virtual const ClassIdent *GetIdent() const       { return(&identifier); }

   //: Visit a StrChunk, with no extent constraints imposed by parent.
   virtual void visitStrChunk(const StrChunkPtr &chunk)
      throw(halt_visitation) = 0;
   //: Visit a StrChunk, with extent constraints imposed by parent.
   virtual void visitStrChunk(const StrChunkPtr &chunk,
			      const LinearExtent &used)
      throw(halt_visitation) = 0;
   //: Visit some raw data in a StrChunk.
   virtual void visitDataBlock(const void *start, size_t len)
      throw(halt_visitation) = 0;

   //: Call the StrChunk's 'acceptVisitor' method to visit a StrChunk's
   //: children.
   void call_acceptVisitor(const StrChunkPtr &chnk)
      throw(halt_visitation);
};

#endif
