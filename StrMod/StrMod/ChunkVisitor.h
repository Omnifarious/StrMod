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

/** \class ChunkVisitor ChunkVisitor.h StrMod/ChunkVisitor.h
 * The interface for a StrChunk visitor.
 *
 * Part of an implementation of the <a
 * href="http://exciton.cs.oberlin.edu/javaresources/DesignPatterns/VisitorPattern.htm">Visitor
 * pattern</a> for traversing StrChunk DAGs.
 *
 * A StrChunk may be visited many times because the StrChunk containment
 * hierarchy is a DAG.  This means that a given StrChunk can be contained
 * through more than one path, though it can never contain itself.  Because a
 * StrChunk can contain 'substrings' of other StrChunks, each time a StrChunk
 * is visited, it may have different constraints stating which part of the
 * data or StrChunks it contains are visible to the parent.
 *
 * Derived classes should provide some sort of 'visit' method that takes at
 * least the top level chunk to be visited as an argument.  It isn't provided
 * here because many visitor classes will have build up and tear down
 * operations to perform before visiting.
 */
class ChunkVisitor : public Protocol {
   friend class StrChunk;
 public:
   //! An exception to allow the visitor to halt the traversal.
   class halt_visitation : public exception {
   };
   static const STR_ClassIdent identifier;

   //! Do nothing constructor for interface class.
   ChunkVisitor()                                   { }
   //! Do nothing virtual destructor for interface class.
   virtual ~ChunkVisitor()                          { }

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || Protocol::AreYouA(cid));
   }

 protected:
   virtual const ClassIdent *i_GetIdent() const       { return(&identifier); }

   /** \name StrChunk visit functions
    */
   //@{
   /**
    * Visit a StrChunk, with no extent constraints imposed by parent.
    */
   virtual void visitStrChunk(const StrChunkPtr &chunk)
      throw(halt_visitation) = 0;
   /**
    * Visit a StrChunk, with extent constraints imposed by parent.
    */
   virtual void visitStrChunk(const StrChunkPtr &chunk,
			      const LinearExtent &used)
      throw(halt_visitation) = 0;
   //@}
   //! Visit some raw data in a StrChunk.
   virtual void visitDataBlock(const void *start, size_t len)
      throw(halt_visitation) = 0;

   /**
    * Call the StrChunk's 'acceptVisitor' method to visit a StrChunk's
    * children.
    *
    * This exists so derived classes will have very limited and controlled
    * access to a protected member function of StrChunk.  It always provides
    * <code>this</code> as the visitor argument of StrChunk::acceptVisitor.
    *
    * @param chnk The StrChunk to call acceptVisitor on.
    */
   void call_acceptVisitor(const StrChunkPtr &chnk)
      throw(halt_visitation);
};

#endif
