#ifndef _STR_StrChunkPtr_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.2  1996/07/05 19:46:25  hopper
// Various changed to make implementing StrChunkPtrT template easier and
// more efficient.
//
// Revision 1.1  1996/06/29 06:55:50  hopper
// New class StrChunkPtr that acts as a reference counted pointer to
// StrChunk
//

#if !(defined(CHUNK_PTR_DEBUG) || defined(CHUNK_PTR_CC))
#  ifndef NDEBUG
#     define NDEBUG
#  else
#     define CHUNK_PTR_NDEBUG_SET
#  endif
#endif

#include <LCore/Object.h>
#include <StrMod/STR_ClassIdent.h>
#include <assert.h>

#define _STR_StrChunkPtr_H_

#if defined(NDEBUG) || defined(CHUNK_PTR_CC)
#  define inline_ inline
#else
#  define inline_
#endif

class StrChunk;

class StrChunkPtr : public Object {
 public:
   static const STR_ClassIdent identifier;

   inline StrChunkPtr(StrChunk *stptr = 0);
   inline StrChunkPtr(const StrChunkPtr &b);
   inline virtual ~StrChunkPtr();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline_ StrChunk &operator *() const;
   inline_ StrChunk *operator ->() const;

   inline StrChunk *GetPtr() const;
   inline void ReleasePtr(bool deleteref = true);

   inline operator bool() const;

   inline const StrChunkPtr &operator =(const StrChunkPtr &b);
   inline const StrChunkPtr &operator =(StrChunk *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   StrChunk *i_GetPtr() const                          { return(ptr); }
   inline void i_ReleasePtr(bool deleteref);
   virtual StrChunk *i_CheckType(StrChunk *p) const    { return(0); }
   void i_SetPtr(StrChunk *p, bool deleteref = true);

 private:
   StrChunk *ptr;
};

//-----------------------------inline functions--------------------------------

inline StrChunkPtr::StrChunkPtr(StrChunk *stptr = 0) : ptr(0)
{
   if (stptr) {
      i_SetPtr(stptr);
   }
}

inline StrChunkPtr::StrChunkPtr(const StrChunkPtr &b) : ptr(0)
{
   i_SetPtr(b.GetPtr());
}

inline StrChunkPtr::~StrChunkPtr()
{
   ReleasePtr(true);
}

inline int StrChunkPtr::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

#if defined(NDEBUG) || defined(CHUNK_PTR_CC)
inline_ StrChunk &StrChunkPtr::operator *() const
{
   assert(i_GetPtr() != 0);

   return(*i_GetPtr());
}

inline_ StrChunk *StrChunkPtr::operator ->() const
{
   assert(i_GetPtr() != 0);

   return(i_GetPtr());
}
#endif

inline StrChunk *StrChunkPtr::GetPtr() const
{
   return(i_GetPtr());
}

inline void StrChunkPtr::ReleasePtr(bool deleteref = true)
{
   i_ReleasePtr(deleteref);
}

inline StrChunkPtr::operator bool() const
{
   return(i_GetPtr() != 0);
}

inline const StrChunkPtr &StrChunkPtr::operator =(const StrChunkPtr &b)
{
   if (this != &b) {
      StrChunk *p = b.GetPtr();

      operator =(p);
   }
   return(*this);
}

inline const StrChunkPtr &StrChunkPtr::operator =(StrChunk *b)
{
   b = i_CheckType(b);
   if (GetPtr() != b) {
      i_SetPtr(b);
   }
   return(*this);
}

inline void StrChunkPtr::i_ReleasePtr(bool deleteref)
{
   i_SetPtr(0, deleteref);
}

#if !(defined(CHUNK_PTR_DEBUG) || defined(CHUNK_PTR_CC))
#  ifndef CHUNK_PTR_NDEBUG_SET
#     undef NDEBUG
#  else
#     undef CHUNK_PTR_NDEBUG_SET
#  endif
#endif

#endif
