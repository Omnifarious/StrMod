#ifndef _STR_StrChunkPtrT_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/07/05 18:39:48  hopper
// New type to handle StrChunkPtr's that need to point to more specific
// types than StrChunk.
//

#include <StrMod/StrChunkPtr.h>

#define _STR_StrChunkPtrT_H_

template <class Chunk>
class StrChunkPtrT : public StrChunkPtr {
 public:
   inline StrChunkPtrT(Chunk *stptr = 0) : StrChunkPtr(stptr)               { }
   inline StrChunkPtrT(const StrChunkPtrT<Chunk> &b) : StrChunkPtr(b)       { }

   inline Chunk &operator *() const;
   inline Chunk *operator ->() const;

   inline Chunk *GetPtr() const;

   inline const StrChunkPtrT<Chunk> &operator =(const StrChunkPtr &b);
   inline const StrChunkPtrT<Chunk> &operator =(StrChunk *b);
   inline const StrChunkPtrT<Chunk> &operator =(const StrChunkPtrT<Chunk> &b);
   inline const StrChunkPtrT<Chunk> &operator =(Chunk *b);

 protected:
   inline virtual StrChunk *i_CheckType(StrChunk *p) const;
};

//-----------------------------inline functions--------------------------------

template <class Chunk>
inline Chunk &StrChunkPtrT<Chunk>::operator *() const
{
   return(static_cast<Chunk &>(StrChunkPtr::operator *()));
}

template <class Chunk>
inline Chunk *StrChunkPtrT<Chunk>::operator ->() const
{
   return(static_cast<Chunk *>(StrChunkPtr::operator ->()));
}

template <class Chunk>
inline Chunk *StrChunkPtrT<Chunk>::GetPtr() const
{
   return(static_cast<Chunk *>(i_GetPtr()));
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(const StrChunkPtr &b)
{
   StrChunkPtr::operator =(b);
   return(*this);
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(StrChunk *b)
{
   StrChunkPtr::operator =(b);
   return(*this);
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(const StrChunkPtrT<Chunk> &b)
{
   if (this != &b) {
      Chunk *p = b.GetPtr();

      operator =(p);
   }
   return(*this);
}

template <class Chunk>
inline const StrChunkPtrT<Chunk> &
StrChunkPtrT<Chunk>::operator =(Chunk *b)
{
   if (GetPtr() != b) {
      i_SetPtr(b);
   }
   return(*this);
}

template <class Chunk>
inline StrChunk *StrChunkPtrT<Chunk>::i_CheckType(StrChunk *p) const
{
   if ((p == 0) || p->AreYouA(Chunk::identifier)) {
      return(p);
   } else {
      assert(0 && "Bad pointer cast");
      return(0);
   }
}

#endif
