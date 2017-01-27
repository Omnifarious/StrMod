/* -*-c-file-style: "hopper";-*- */
/* $Header$ */

/*
 * Copyright 2000-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "ChunkIterator.h"
#endif

#include <memory>
#include "StrMod/ChunkIterator.h"
#include "StrMod/UseTrackingVisitor.h"
#include <vector>
#include <algorithm>
#include <cstring>

namespace strmod {
namespace strmod {

/**
 * A bunch of data that can be share among all the StrChunk::__iterator
 * objects for a given StrChunk.
 */
class StrChunk::iterator__::shared : public StrChunk::private_iter_state
{
 public:
   typedef ::std::shared_ptr<shared> me_ptr_t;
   /**
    * Holds information about where to find a chunk of data and how big it is.
    */
   struct rawdata {
      const void *base_;  ///< Location of data area
      size_t len_;  ///< Length of data area
   };
   StrChunkPtr root_;  ///< The StrChunk I'm the share iterator data area for.
   rawdata *dataexts_;  ///< A list of the data chunks in root_.
   unsigned int numexts_; ///< The number of data chunks in root_.
   unsigned int length_;  ///< The total length of all of the data chunks in root_.  Should be equal to root_->Length()

   inline explicit shared(const StrChunkPtr &root);
   /**
    * Set the special StrChunk storage pointer back to NULL if it was pointing
    * at me.
    */
   ~shared()
   {
      delete[] dataexts_;
   }
   /**
    * Pulls out the mysterious void * inside StrChunk to check to see if
    * another StrChunk::iterator__ has left us with a gift of a shared data
    * section we can use.
    */
   static me_ptr_t forStrChunk(const StrChunkPtr &chnk) {
      typedef StrChunk::private_iter_state pis_t;
      typedef ::std::shared_ptr<pis_t> pis_ptr_t;
      pis_ptr_t iter_storage(chnk->iter_storage_.lock());
      me_ptr_t result;

      result = ::std::dynamic_pointer_cast<shared>(iter_storage);
      if (!result) {
         result.reset(new shared(chnk));
         if (!iter_storage) {
            chnk->iter_storage_ = result;
         }
      }
      return result;
   }
};

/**
 * The ChunkVisitor that gathers data for the StrChunk::iterator__ to use.
 */
class StrChunk::iterator__::ExtVisitor : public UseTrackingVisitor {
 public:
   //! ChunkVisitors never have very interesting constructors
   // Do ignore zeros though.  When iterating over data, zero length chunks
   // and data extents are pointless.
   ExtVisitor() : UseTrackingVisitor(true)  { }
   //! And rarely interesting destructors either.
   virtual ~ExtVisitor()                    { }

   /**
    * Visit the StrChunk DAG rooted at root, filling up the shared data
    * structure with what I find.
    *
    * @param root The StrChunk to visit.
    *
    * @param dataexts A reference to a pointer to a list of data extents.  Set
    * to point at the new list after it's all ben figured out.
    *
    * @param numexts A reference to an int that will be filled with the number
    * of data extents I found, and therefor how many data extents dataexts now
    * points at.
    */
   void visit(const StrChunkPtr &root,
              shared::rawdata *&dataexts, unsigned int &numexts)
   {
      extvec_.clear();
      try {
         startVisit(root);
         numexts = extvec_.size();
         if (numexts > 0)
         {
            dataexts = new shared::rawdata[numexts];
            copy(extvec_.begin(), extvec_.end(), dataexts);
         }
      } catch (...) {
         extvec_.clear();
         throw;
      }
      extvec_.clear();
   }

 protected:
   /*!
    * I don't care about chunks, just data (because the iterator has to
    * iterate over the data) so do nothing when told about a chunk.
    */
   virtual void use_visitStrChunk(const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(halt_visitation)                { }

   /*!
    * Add this new chunk of data to our list.
    */
   virtual void use_visitDataBlock(const void *start, size_t len,
                                   const void *realstart, size_t reallen)
      throw(halt_visitation)
   {
      // Many routines depend on this if statement to ensure that there are no
      // zero length extents.
      if (len <= 0)
      {
         return;
      }

      shared::rawdata data = {start, len};

      extvec_.push_back(data);
   }

 private:
   std::vector<shared::rawdata> extvec_;
};

//---

/**
 * Create a shared data area and set up the funny void * storage area in
 * StrChunk to hold it.
 */
inline StrChunk::iterator__::shared::shared(const StrChunkPtr &root)
     : root_(root), dataexts_(0), numexts_(0),
       length_(root->Length())
{
   assert(root);
   ExtVisitor visitor;
   visitor.visit(root, dataexts_, numexts_);
}

//---

StrChunk::iterator__::iterator__()
     : abspos_(0), extpos_(0), extlast_(0), curext_(0), extbase_(0)
{
}

StrChunk::iterator__::iterator__(const StrChunkPtr &chnk)
     : shared_(shared::forStrChunk(chnk)),
       abspos_(0), extpos_(0), extlast_(0), curext_(0), extbase_(0)
{
   assert(chnk);
   moveToBegin();
}

StrChunk::iterator__::iterator__(const ::std::shared_ptr<shared> &sh)
     : shared_(sh), abspos_(0), extpos_(0), extlast_(0),
       curext_(0), extbase_(0)
{
   if (shared_)
   {
      moveToBegin();
   }
}

StrChunk::iterator__::iterator__(const iterator__ &other)
     : shared_(other.shared_), abspos_(other.abspos_), extpos_(other.extpos_),
       extlast_(other.extlast_), curext_(other.curext_),
       extbase_(other.extbase_)
{
}

StrChunk::iterator__::~iterator__()
{
}

inline bool StrChunk::iterator__::isFor(const StrChunkPtr &chnk) const
{
   return shared_ && (shared_->root_ == chnk);
}

const StrChunk::iterator__ &
StrChunk::iterator__::operator =(const iterator__ &other)
{
   shared_ = other.shared_;
   abspos_ = other.abspos_;
   extpos_ = other.extpos_;
   extlast_ = other.extlast_;
   extbase_ = other.extbase_;
   curext_ = other.curext_;
   return *this;
}

bool StrChunk::iterator__::isEqual(const iterator__ &other) const
{
   if ((shared_ == other.shared_) ||
       (shared_ && other.shared_ &&
        (shared_->root_ == other.shared_->root_)))
   {
      return abspos_ == other.abspos_;
   }
   return false;
}
/*!
 * Returns a comparison of position within a StrChunk if the two iterators
 * point at the same StrChunk.
 *
 * If they don't point at the same StrChunk it makes a nonsensical comparison
 * that is guaranteed to be stable and not register two such iterators as
 * neither being both less than and greater than each other nor being not less
 * than and not greature than each other.
 */
bool StrChunk::iterator__::isLessThan(const iterator__ &other) const
{
   return abspos_ < other.abspos_ || \
      ((shared_ ? shared_->root_.get() : 0) <
       (other.shared_ ? shared_->root_.get() : 0));
}

/*!
 * @return The number of bytes between two iterators.
 *
 * Returns a silly value if the two iterators don't point at the same
 * StrChunk.  I believe that STL leaves this behavior undefined, so the silly
 * value is as good as anything else.
 */
StrChunk::iterator__::difference_type
StrChunk::iterator__::distance(const iterator__ &other) const
{
   if ((shared_ == other.shared_) ||
       (shared_ && other.shared_ &&
        (shared_->root_ == other.shared_->root_)))
   {
      return abspos_ - other.abspos_;
   }
   else
   {
      // lamer speak for loselose.  What you did if you got this value
      // back.  :-) Maybe this ought to be an exception.
      return 0x10531053;
   }
}

static const unsigned char junk = 'G';

void StrChunk::iterator__::moveToEnd()
{
   if (!shared_)
   {
      return;
   }
   shared &shrd = *shared_;
   abspos_ = shrd.length_;
   curext_ = shrd.numexts_;
   extpos_ = 0;
   extbase_ = &junk;
   extlast_ = 0;
}

void StrChunk::iterator__::moveToBegin()
{
   if (!shared_)
   {
      return;
   }
   shared &shrd = *shared_;
   abspos_ = 0;
   curext_ = 0;
   extpos_ = 0;
   if (shrd.length_ > 0)
   {
      assert(shrd.numexts_ > 0);
      extbase_ = static_cast<const unsigned char *>(shrd.dataexts_[0].base_);
      extlast_ = shrd.dataexts_[0].len_ - 1;
   }
   else
   {
      extbase_ = &junk;
      extlast_ = 0;
   }
}

void StrChunk::iterator__::move_forward_complex()
{
   assert(extpos_ >= extlast_);
   if (extpos_ < extlast_)
   {
      ++extpos_;
      return;
   }
   if (shared_ == NULL)
   {
      return;
   }
   shared &shrd = *shared_;
   if ((shrd.length_ <= 0) || (abspos_ >= (shrd.length_ - 1)))
   {
      moveToEnd();
      return;
   }
   ++abspos_;
   assert(abspos_ < shrd.length_);
   ++curext_;
   assert(curext_ < shrd.numexts_);
   extpos_ = 0;
   extbase_ = static_cast<const unsigned char *>(shrd.dataexts_[curext_].base_);
   extlast_ = shrd.dataexts_[curext_].len_;
   assert(extlast_ > 0);
   // extlast was the length of the buffer, now it's the index of the last
   // element.
   --extlast_;
}

void StrChunk::iterator__::move_backward_complex()
{
   assert(extpos_ == 0);
   if (extpos_ != 0)
   {
      --extpos_;
      return;
   }
   if (shared_ == NULL)
   {
      return;
   }
   shared &shrd = *shared_;
   if (abspos_ <= 1)
   {
      moveToBegin();
   }
   else
   {
      assert(curext_ > 0);
      --abspos_;
      --curext_;
      extbase_ = static_cast<const unsigned char *>(shrd.dataexts_[curext_].base_);
      extlast_ = shrd.dataexts_[curext_].len_;
      assert(extlast_ > 0);
      // extlast was the length of the buffer, now it's the index of the last
      // element.
      --extlast_;
      extpos_ = extlast_;
   }
}

//---

StrChunk::iterator__ StrChunk::begin()
{
   return iterator__(shared_from_this());
}


StrChunk::iterator__ StrChunk::end()
{
   iterator__ tmp(shared_from_this());
   tmp.moveToEnd();
   return tmp;
}

}  // End namespace strmod
}  // End namespace strmod
