#ifndef _STR_GraphVizVisitor_H_  // -*-mode: c++; c-file-style: "gmtellemtel";-*-

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

//: Generates output suitable for AT&T's Open Source GraphViz program.
// http://www.research.att.com/sw/tools/graphviz/
class GraphVizVisitor : public UseTrackingVisitor {
 public:
   static const STR_ClassIdent identifier;

   GraphVizVisitor() : out_(0)      { }
   virtual ~GraphVizVisitor()       { }

   void visit(const StrChunkPtr &root, ostream &out);

 protected:
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
