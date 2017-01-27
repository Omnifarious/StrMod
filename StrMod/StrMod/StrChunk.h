#ifndef _STR_StrChunk_H_  // -*-c++-*-

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
#pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.

#include <cassert>
#include <cstddef>
#include <memory>
#include <StrMod/ChunkVisitor.h>

#define _STR_StrChunk_H_

namespace strmod {
namespace strmod {

class LinearExtent;

/** \class StrChunk StrChunk.h StrMod/StrChunk.h
 * An interface to a reference counted chunk of data.
 *
 * The chunk of data is actually held by the derived classes.  Some chunks of
 * data even consist of other StrChunks.  This is an instance of the <A HREF="http://exciton.cs.oberlin.edu/javaresources/DesignPatterns/composite.htm">Composite</A>
 * design pattern.
 *
 * StrChunks are supposed to be immutable after they're created.  Often
 * derived classes will have methods for updating the data held within, but
 * StrChunks don't, and they're what the outside world sees.
 *
 * StrChunks are reference counted.  Use the StrChunkPtr class if you want a
 * nice smart pointer that will automatically maintain the reference count and
 * delete the chunk when necessary.
 *
 * StrChunks are reference counted because the containment hierarchy for them
 * is a DAG.  Just because the StrChunk is found by traversing a tree rooted
 * at a particular StrChunk doesn't mean that same StrChunk can't be found in
 * a different tree.  It might even be found twice in the same tree.  This
 * complexity requires reference counts for tractable resource handling.
 */
class StrChunk : public ::std::enable_shared_from_this<StrChunk>
{
   friend class ChunkVisitor;
 public:
   class iterator__;
   friend class iterator__;
   //! Give the type an STL name.
   typedef iterator__ const_iterator;

   //! Not much to talk about.
   virtual ~StrChunk()                                 { }

   //! Number of octets this chunk takes up.  May be deprecated.
   virtual unsigned int Length() const = 0;

   //@{
   /**
    * Get an STL style const bidirectional iterator.
    *
    * This iterator will range over all the octects of type U1Byte that are in
    * the chunk.
    */
   const_iterator begin();
   const_iterator end();
   //@}

 protected:
   //! Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation) = 0;

   //@{
   /**
    * These are helper functions so derived classes have limited access to
    * ChunkVisitor methods.
    */
   inline void call_visitStrChunk(ChunkVisitor &visitor,
                                  const StrChunkPtr &chunk)
      throw(ChunkVisitor::halt_visitation);
   inline void call_visitStrChunk(ChunkVisitor &visitor,
                                  const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(ChunkVisitor::halt_visitation);
   inline void call_visitDataBlock(ChunkVisitor &visitor,
                                   void *start, size_t len)
      throw(ChunkVisitor::halt_visitation);
   //@}

 private:
   class private_iter_state {
    public:
      virtual ~private_iter_state() {}
   };
   mutable ::std::weak_ptr<private_iter_state> iter_storage_;
};

//------------------------inline functions for StrChunk------------------------

inline void StrChunk::call_visitStrChunk(ChunkVisitor &visitor,
                                         const StrChunkPtr &chunk)
   throw(ChunkVisitor::halt_visitation)
{
   visitor.visitStrChunk(chunk);
}

inline void StrChunk::call_visitStrChunk(ChunkVisitor &visitor,
                                         const StrChunkPtr &chunk,
                                         const LinearExtent &used)
   throw(ChunkVisitor::halt_visitation)
{
   visitor.visitStrChunk(chunk, used);
}

inline void StrChunk::call_visitDataBlock(ChunkVisitor &visitor,
                                          void *start, size_t len)
   throw(ChunkVisitor::halt_visitation)
{
   visitor.visitDataBlock(start, len);
}

}  // namespace strmod
}  // namespace strmod

#endif
