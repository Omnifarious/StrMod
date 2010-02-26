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
#include <StrMod/StrChunkPtr.h>
#include <LCore/GenTypes.h>
#include <iterator>

#define _STR_ChunkIterator_H_

namespace strmod {
namespace strmod {

/** \class StrChunk::__iterator ChunkIterator.h StrMod/ChunkIterator.h
 * The const_iterator class for StrChunk.
 */
class StrChunk::iterator__
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
   iterator__();
   //! Creates an interator pointat the beginning of a StrChunk.
   iterator__(const StrChunkPtr &chnk);
   //! Creates an iterator using the shared data already generated.
   iterator__(const ::std::tr1::shared_ptr<shared> &sh);
   //! Copies one iterator to another.  Manages reference count on shared area.
   iterator__(const iterator__ &other);
   //! Destroys an iterator, possibly destroying shared data too if refcount is only 1
   ~iterator__();

   //! Am I an iterator over this chunk?
   bool isFor(const StrChunkPtr &chnk) const;

   //! Am I equal to other?  Do I iterator over the same StrChunk and am I at
   //! the same spot?
   bool isEqual(const iterator__ &other) const;
   //! Am I less than other?
   bool isLessThan(const iterator__ &other) const;

   //! How many characters are there between me and another iterator?
   difference_type distance(const iterator__ &other) const;

   //! Make me into a copy of another iterator.
   const iterator__ &operator =(const iterator__ &other);

   //! Using this operator will almost certainly result in a compiler error.
   inline pointer operator->() const;
   //! What character am I looking at?
   inline reference operator *() const;

   //! Move forward one.  Prefix operator, so it's more efficient.
   inline const iterator__ &operator ++();
   //! Move forward one.  Postfix operator, so it's less efficient.
   inline const iterator__ operator ++(int);
   //! Move to one past the last character.
   void moveToEnd();
   //! Move backward one.  Prefix operator, so it's more efficient.
   inline const iterator__ &operator --();
   //! Move backward one.  Postfix operator, so it's less efficient.
   inline const iterator__ operator --(int);
   //! Move to the first character (which may be one past the last character
   //! if the StrChunk is empty.
   void moveToBegin();

 private:
   class ExtVisitor;
   friend class ExtVisitor;
   ::std::tr1::shared_ptr<shared> shared_;
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

inline StrChunk::iterator__::pointer
StrChunk::iterator__::operator->() const
{
   return(&(extbase_[extpos_]));
}

inline StrChunk::iterator__::reference
StrChunk::iterator__::operator *() const
{
   return(extbase_[extpos_]);
}

inline const StrChunk::iterator__ &StrChunk::iterator__::operator ++()
{
   move_forward();
   return(*this);
}

inline const StrChunk::iterator__ StrChunk::iterator__::operator ++(int)
{
   iterator__ tmp(*this);
   move_forward();
   return(tmp);
}

inline const StrChunk::iterator__ &StrChunk::iterator__::operator --()
{
   move_backward();
   return(*this);
}

inline const StrChunk::iterator__ StrChunk::iterator__::operator --(int)
{
   iterator__ tmp(*this);
   move_backward();
   return(tmp);
}

inline void StrChunk::iterator__::move_forward()
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

inline void StrChunk::iterator__::move_backward()
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

//--

inline bool
operator ==(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(a.isEqual(b));
}

inline bool
operator !=(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(! a.isEqual(b));
}

inline bool
operator <(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(a.isLessThan(b));
}

inline bool
operator >(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(b.isLessThan(a));
}

inline bool
operator <=(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(! b.isLessThan(a));
}

inline bool
operator >=(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(! a.isLessThan(b));
}

inline StrChunk::iterator__::difference_type
operator -(const StrChunk::iterator__ &a, const StrChunk::iterator__ &b)
{
   return(a.distance(b));
}

};  // namespace strmod
};  // namespace strmod

#endif
