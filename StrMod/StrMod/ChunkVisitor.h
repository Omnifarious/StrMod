#ifndef _STR_ChunkVisitor_H_  // -*-c++-*-

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
