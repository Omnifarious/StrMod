#ifndef _STR_GraphVizVisitor_H_  // -*-mode: c++; c-file-style: "gmtellemtel";-*-

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

#include <StrMod/UseTrackingVisitor.h>
#include <set>
#include <utility>

#define _STR_GraphVizVisitor_H_

class ostream;
class BufferChunk;

/** \class GraphVizVisitor GraphVizVisitor.h StrMod/GraphVizVisitor.h
 * Generates output suitable for AT&T's Open Source GraphViz program, found at
 * http://www.research.att.com/sw/tools/graphviz/
 */
class GraphVizVisitor : public UseTrackingVisitor {
 public:
   static const STR_ClassIdent identifier;

   //! Constructor, doesn't do much.
   GraphVizVisitor() : out_(0)      { }
   //! Destructor, also doesn't do much.
   virtual ~GraphVizVisitor()       { }

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || UseTrackingVisitor::AreYouA(cid));
   }

   /**
    * Visits the chunk DAG printing out a GraphViz parsable graph description,
    * returning a StrChunk containing the data for the chunk DAG.
    */
   const StrChunkPtr visit(const StrChunkPtr &root, ostream &out);

 protected:
   virtual const ClassIdent *i_GetIdent() const       { return(&identifier); }

   virtual void use_visitStrChunk(const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(halt_visitation);

   virtual void use_visitDataBlock(const void *start, size_t len,
                                   const void *realstart, size_t reallen)
      throw(halt_visitation);

   void printData(const void *data, size_t len);

 private:
   typedef pair<const void *, const void *> edge_t;
   typedef set<edge_t> edgeset_t;
   ostream *out_;
   BufferChunk *data_;
   size_t rootpos_;
   edgeset_t edges_;
};

#endif