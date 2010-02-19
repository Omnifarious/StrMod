#ifndef _STR_ChunkIterator_H_  // -*-c++-*-

/*
 * Copyright 2000-2002 by Eric M. Hopper <hopper@omnifarious.org>
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

#include <StrMod/StrChunk.h>
#include <LCore/GenTypes.h>
#include <iterator>

#define _STR_ChunkIterator_H_

namespace strmod {
namespace strmod {

/** \clsss StrChunk::__iterator ChunkIterator.h StrMod/ChunkIterator.h
 * The const_iterator class for StrChunk.
 */
class StrChunk::__iterator
{
 private:
   class shared;
   friend class shared;
   typedef lcore::U1Byte U1Byte;

 public:
   typedef ::std::bidirectional_iterator_tag iterator_category;
   typedef U1Byte value_type;
   typedef int difference_type;
   typedef const U1Byte *pointer;
   typedef const U1Byte &reference;

   //! Creates an iterator that's always at the end()
   __iterator();
   //! Creates an interator pointat the beginning of a StrChunk.
   __iterator(const StrChunkPtr &chnk);
   //! Creates an iterator using the shared data already generated.
   __iterator(shared *sh);
   //! Copies one iterator to another.  Manages reference count on shared area.
   __iterator(const __iterator &other);
   //! Destroys an iterator, possibly destroying shared data too if refcount is only 1
   ~__iterator();

   //! Am I an iterator over this chunk?
   bool isFor(const StrChunkPtr &chnk) const;

   //! Am I equal to other?  Do I iterator over the same StrChunk and am I at
   //! the same spot?
   bool isEqual(const __iterator &other) const;
   //! Am I less than other?
   bool isLessThan(const __iterator &other) const;

   //! How many characters are there between me and another iterator?
   difference_type distance(const __iterator &other) const;

   //! Make me into a copy of another iterator.
   const __iterator &operator =(const __iterator &other);

   //! Using this operator will almost certainly result in a compiler error.
   inline pointer operator->() const;
   //! What character am I looking at?
   inline reference operator *() const;

   //! Move forward one.  Prefix operator, so it's more efficient.
   inline const __iterator &operator ++();
   //! Move forward one.  Postfix operator, so it's less efficient.
   inline const __iterator operator ++(int);
   //! Move to one past the last character.
   void moveToEnd();
   //! Move backward one.  Prefix operator, so it's more efficient.
   inline const __iterator &operator --();
   //! Move backward one.  Postfix operator, so it's less efficient.
   inline const __iterator operator --(int);
   //! Move to the first character (which may be one past the last character
   //! if the StrChunk is empty.
   void moveToBegin();

 protected:
   //! Set the secret storage compartment in a StrChunk to val.
   inline static void setStorage(StrChunk &chnk, void *val);
   //! Get what's in the secret storage compartment inside a StrChunk.
   inline static void *getStorage(StrChunk &chnk);

 private:
   class ExtVisitor;
   friend class ExtVisitor;
   shared *shared_;
   unsigned int abspos_;
   unsigned int extpos_;
   unsigned int extlast_;  // Index of last element (not length) of extbase_.
   unsigned int curext_;
   const unsigned char *extbase_;

   inline void move_forward();
   void move_forward_complex();
   inline void move_backward();
   void move_backward_complex();
};

//-----------------------------inline functions--------------------------------

inline StrChunk::__iterator::pointer
StrChunk::__iterator::operator->() const
{
   return(&(extbase_[extpos_]));
}

inline StrChunk::__iterator::reference
StrChunk::__iterator::operator *() const
{
   return(extbase_[extpos_]);
}

inline const StrChunk::__iterator &StrChunk::__iterator::operator ++()
{
   move_forward();
   return(*this);
}

inline const StrChunk::__iterator StrChunk::__iterator::operator ++(int)
{
   __iterator tmp(*this);
   move_forward();
   return(tmp);
}

inline const StrChunk::__iterator &StrChunk::__iterator::operator --()
{
   move_backward();
   return(*this);
}

inline const StrChunk::__iterator StrChunk::__iterator::operator --(int)
{
   __iterator tmp(*this);
   move_backward();
   return(tmp);
}

inline void StrChunk::__iterator::move_forward()
{
   if (extpos_ < extlast_)
   {
      ++extpos_;
      ++abspos_;
   }
   else
   {
      move_forward_complex();
   }
}

inline void StrChunk::__iterator::move_backward()
{
   if (extpos_ > 0)
   {
      --extpos_;
      --abspos_;
   }
   else
   {
      move_backward_complex();
   }
}

inline void StrChunk::__iterator::setStorage(StrChunk &chnk, void *val)
{
   chnk.iter_storage = val;
}

inline void *StrChunk::__iterator::getStorage(StrChunk &chnk)
{
   return(chnk.iter_storage);
}

//--

inline bool
operator ==(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(a.isEqual(b));
}

inline bool
operator !=(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(! a.isEqual(b));
}

inline bool
operator <(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(a.isLessThan(b));
}

inline bool
operator >(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(b.isLessThan(a));
}

inline bool
operator <=(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(! b.isLessThan(a));
}

inline bool
operator >=(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(! a.isLessThan(b));
}

inline StrChunk::__iterator::difference_type
operator -(const StrChunk::__iterator &a, const StrChunk::__iterator &b)
{
   return(a.distance(b));
}

};  // namespace strmod
};  // namespace strmod

#endif
