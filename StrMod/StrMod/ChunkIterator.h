#ifndef _STR_ChunkIterator_H_  // -*-c++-*-

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

#include <StrMod/StrChunk.h>
#include <LCore/GenTypes.h>
#include <iterator>

#define _STR_ChunkIterator_H_

class StrChunk::__iterator :
   public bidirectional_iterator<const U1Byte, int>
{
 private:
   class shared;
   friend class shared;

 public:
   __iterator();
   __iterator(const StrChunkPtr &chnk);
   __iterator(shared *sh);
   __iterator(const __iterator &other);
   ~__iterator();

   bool isEqual(const __iterator &other) const;
   bool isLessThan(const __iterator &other) const;

   difference_type distance(const __iterator &other) const;

   const __iterator &operator =(const __iterator &other);

   inline pointer operator->() const;
   inline reference operator *() const;

   inline const __iterator &operator ++();
   inline const __iterator operator ++(int);
   void moveToEnd();
   inline const __iterator &operator --();
   inline const __iterator operator --(int);
   void moveToBegin();

 protected:
   inline static void setStorage(StrChunk &chnk, void *val);
   inline static void *getStorage(StrChunk &chnk);

 private:
   class ExtVisitor;
   unsigned int abspos_;
   unsigned int extpos_;
   unsigned int extlast_;  // Index of last element (not length) of extbase_.
   unsigned int curext_;
   const unsigned char *extbase_;
   shared *shared_;

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

#endif
