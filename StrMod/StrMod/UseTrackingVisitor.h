#ifndef _STR_UseTrackingVisitor_H_  // -*-mode: c++; c-file-style: "hopper";-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/ChunkVisitor.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/LinearExtent.h>
#include <cstddef>

#define _STR_UseTrackingVisitor_H_

class UseTrackingVisitor : public ChunkVisitor {
 public:
   static const STR_ClassIdent identifier;

   UseTrackingVisitor(bool ignorezeros = false);
   virtual ~UseTrackingVisitor();

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || ChunkVisitor::AreYouA(cid));
   }

 protected:
   virtual const ClassIdent *GetIdent() const       { return(&identifier); }

   //: This is the function called by the UseTracking machinery.
   //: <strong>Don't overload functions that don't start with
   //: <code>use_</code></strong>.
   virtual void use_visitStrChunk(const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(halt_visitation) = 0;

   //: This is the function called by the UseTracking machinery.
   //: <strong>Don't overload functions that don't start with
   //: <code>use_</code></strong>.
   virtual void use_visitDataBlock(const void *start, size_t len,
                                   const void *realstart, size_t reallen)
      throw(halt_visitation) = 0;

   inline const StrChunkPtr &getParent() const;
   inline const LinearExtent::off_t parentOffset() const;

   inline void startVisit(const StrChunkPtr &root);

 private:
   LinearExtent curext_;
   LinearExtent::off_t curpos_;
   StrChunkPtr curchnk_;
   const bool ignorezeros_;

   //: Don't overload this function in derived classes!  (Wish I had Java's
   //: 'final'.)
   virtual void visitStrChunk(const StrChunkPtr &chunk)
      throw(halt_visitation);
   //: Don't overload this function in derived classes!  (Wish I had Java's
   //: 'final'.)
   virtual void visitStrChunk(const StrChunkPtr &chunk,
			      const LinearExtent &used)
      throw(halt_visitation);
   //: Don't overload this function in derived classes!  (Wish I had Java's
   //: 'final'.)
   virtual void visitDataBlock(const void *start, size_t len)
      throw(halt_visitation);

   const LinearExtent computeUsed(const LinearExtent &used);
   void do_acceptVisitor(const StrChunkPtr &chunk, const LinearExtent &chunkext)
      throw(halt_visitation);
};

//-----------------------------inline functions--------------------------------

inline const StrChunkPtr &UseTrackingVisitor::getParent() const
{
   return(curchnk_);
}

inline const LinearExtent::off_t UseTrackingVisitor::parentOffset() const
{
   return(curpos_);
}

inline void UseTrackingVisitor::startVisit(const StrChunkPtr &root)
{
   curpos_ = 0;
   curext_ = LinearExtent::full_extent;
   visitStrChunk(root);
}

#endif
