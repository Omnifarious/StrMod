#ifndef _STR_StrChunkPtrT_H_  // -*-c++-*-

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

template <class Chunk>
class StrChunkPtrT : public StrChunkPtr {
 public:
   typedef StrChunkPtr super1;

   inline StrChunkPtrT(const StrChunkPtrT<Chunk> &b) : super1(b)            { }
   inline StrChunkPtrT(const RefCountPtrT<Chunk> &b) : super1(b.GetPtr())   { }
   inline StrChunkPtrT(Chunk *stptr = 0) : super1(stptr)                    { }

   inline Chunk &operator *() const;
   inline Chunk *operator ->() const;

   inline Chunk *GetPtr() const;

   inline const StrChunkPtrT<Chunk> &operator =(const StrChunkPtrT<Chunk> &b);
   inline const StrChunkPtrT<Chunk> &operator =(const RefCountPtrT<Chunk> &b);
   inline const StrChunkPtrT<Chunk> &operator =(Chunk *b);

 protected:
   inline virtual RC *i_CheckType(RC *p) const;
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
StrChunkPtrT<Chunk>::operator =(const RefCountPtrT<Chunk> &b)
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
inline ReferenceCounting *
StrChunkPtrT<Chunk>::i_CheckType(ReferenceCounting *p) const
{
   return(((p != 0) && p->AreYouA(Chunk::identifier)) ? p : 0);
}

#endif
