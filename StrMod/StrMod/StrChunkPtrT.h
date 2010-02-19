#ifndef _STR_StrChunkPtrT_H_  // -*-c++-*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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

// For log see ../ChangeLog
// 
// Revision 1.1  1996/07/05 18:39:48  hopper
// New type to handle StrChunkPtr's that need to point to more specific
// types than StrChunk.
//

#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif
#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif

#define _STR_StrChunkPtrT_H_

namespace strmod {
namespace strmod {

/** \class StrChunkPtrT StrChunkPtrT.h StrMod/StrChunkPtrT.h
 * A convenience template to point at things derived from StrChunk.
 *
 * This is nice to have since the inheritance relationship between
 * StrChunkPtrT and StrChunk mirrors the inheritance relationship between
 * StrChunk and any of it's subclasses.  This allows us to use a StrChunkPtrT
 * when we care about the type of a StrChunk and still pass it into methods
 * requiring a <code>const StrChunkPtr &</code> without a type conversion.
 *
 * Note that this class (and none of my other template classes) override the
 * various Protocol type identification methods.  This is because the LCore
 * type identification system doesn't handle templated types well, and also
 * because of the dubious value of identifying them precisely.  It will still
 * register as being a StrChunkPtr or RefCountPtr.
 */
template <class Chunk>
class StrChunkPtrT : public StrChunkPtr
{
 public:
   //! An easier way to refer to StrChunkPtr.
   typedef StrChunkPtr super1;

   //@{
   /**
    * These all construct a StrChunkPtr from the appropriate type and maintain
    * the reference count to the pointed at StrChunk.
    */
   inline StrChunkPtrT(const StrChunkPtrT<Chunk> &b) : super1(b)            { }
   inline StrChunkPtrT(const lcore::RefCountPtrT<Chunk> &b) : super1(b.GetPtr())   { }
   inline StrChunkPtrT(Chunk *stptr = 0) : super1(stptr)                    { }
   //@}

   //@{
   /**
    * The methods you need to override to do a smart pointer.
    */
   inline Chunk &operator *() const;
   inline Chunk *operator ->() const;
   //@}

   //! A way to get the raw pointer value, just in case.
   inline Chunk *GetPtr() const;

   //@{
   /**
    * These all set a StrChunkPtrs value from the appropriate type and
    * maintain the reference count to the pointed at StrChunk.
    */
   inline const StrChunkPtrT<Chunk> &operator =(const StrChunkPtrT<Chunk> &b);
   inline const StrChunkPtrT<Chunk> &operator =(const lcore::RefCountPtrT<Chunk> &b);
   inline const StrChunkPtrT<Chunk> &operator =(Chunk *b);
   //@}

 protected:
   //! See class RefCountPtr.  Used in ensuring type safety.
   inline virtual lcore::ReferenceCounting *i_CheckType(lcore::ReferenceCounting *p) const;
};

//-----------------------------inline functions--------------------------------

template <class Chunk>
inline Chunk &StrChunkPtrT<Chunk>::operator *() const
{
   return(*GetPtr());
}

template <class Chunk>
inline Chunk *StrChunkPtrT<Chunk>::operator ->() const
{
   return(GetPtr());
}

template <class Chunk>
inline Chunk *StrChunkPtrT<Chunk>::GetPtr() const
{
   return(static_cast<Chunk *>(super1::GetPtr()));
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(const StrChunkPtrT<Chunk> &b)
{
   super1::operator =(b);
   return(*this);
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(const lcore::RefCountPtrT<Chunk> &b)
{
   super1::operator =(b);
   return(*this);
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(Chunk *b)
{
   super1::operator =(b);
   return(*this);
}

template <class Chunk>
inline lcore::ReferenceCounting *
StrChunkPtrT<Chunk>::i_CheckType(lcore::ReferenceCounting *p) const
{
   return(((p != 0) && p->AreYouA(Chunk::identifier)) ? p : 0);
}

}  // namespace strmod
}  // namespace strmod

#endif
