#ifndef _STR_UseTrackingVisitor_H_  // -*-mode: c++; c-file-style: "hopper";-*-

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

#include <StrMod/ChunkVisitor.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/LinearExtent.h>
#include <cstddef>

#define _STR_UseTrackingVisitor_H_

/** \class UseTrackingVisitor UseTrackingVisitor.h StrMod/UseTrackingVisitor.h
 * Simplifies the Visitor interface down so that derived classes don't have to
 * worry about tracking which parts of a StrChunk are actually used.
 */
class UseTrackingVisitor : public ChunkVisitor {
 public:
   static const STR_ClassIdent identifier;

   /**
    * Constructor.
    */
   UseTrackingVisitor(bool ignorezeros = false);
   /**
    * \brief Destructor.  Doesn't do much.
    */
   virtual ~UseTrackingVisitor()                      { }

   virtual int AreYouA(const ClassIdent &cid) const   {
      return((identifier == cid) || ChunkVisitor::AreYouA(cid));
   }

 protected:
   virtual const ClassIdent *i_GetIdent() const       { return(&identifier); }


   /** \name Only overload these
    * <strong>Only overload these functions, not the various private visit*
    * functions down below.</strong>
    */
   //@{
   /**
    * \brief This is the Template Method function to visit a StrChunk.  Called
    * by the UseTracking machinery.
    *
    * @param chunk The chunk being visited.
    *
    * @param extent Which extent of the chunk is used.
    */
   virtual void use_visitStrChunk(const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(halt_visitation) = 0;
   /**
    * \brief This is the Template Method function to visit an actual chunk of
    * data.  Called by the UseTracking machinery.
    *
    * @param start Start of the portion of the data area that's used.
    *
    * @param len Length of the used part of the data area.
    *
    * @param ralstart Start of the actual data area, including the unused
    * portions.
    *
    * @param reallen Length of the actual data area, including the unsused
    * portions.
    */
   virtual void use_visitDataBlock(const void *start, size_t len,
                                   const void *realstart, size_t reallen)
      throw(halt_visitation) = 0;
   //@}

   /**
    * Retrieves the parent StrChunk of the currently visited chunk or piece of
    * data.
    */
   inline const StrChunkPtr &getParent() const;
   /**
    * Retrieves the offset of where the currently visited chunk or piece of
    * data is within its parent.
    */
   inline const LinearExtent::off_t parentOffset() const;

   /**
    * Start a traversal of a chunk DAG.
    */
   inline void startVisit(const StrChunkPtr &root);

 private:
   LinearExtent curext_;
   LinearExtent::off_t curpos_;
   StrChunkPtr curchnk_;
   const bool ignorezeros_;

   //! Don't overload this function in derived classes!  (Wish I had Java's
   //! 'final'.) 
   virtual void visitStrChunk(const StrChunkPtr &chunk)
      throw(halt_visitation);
   //! Don't overload this function in derived classes!  (Wish I had Java's
   //! 'final'.)
   virtual void visitStrChunk(const StrChunkPtr &chunk,
			      const LinearExtent &used)
      throw(halt_visitation);
   //! Don't overload this function in derived classes!  (Wish I had Java's
   //! 'final'.)
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

/*!
 * @param root The root of the DAG to visit.
 */
inline void UseTrackingVisitor::startVisit(const StrChunkPtr &root)
{
   curpos_ = 0;
   curext_ = LinearExtent::full_extent;
   visitStrChunk(root);
}

#endif
