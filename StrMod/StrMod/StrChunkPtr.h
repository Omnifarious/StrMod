#ifndef _STR_StrChunkPtr_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a change log see ../ChangeLog
// 
// Revision 1.5  1998/11/03 00:27:06  hopper
// Changed operator bool to the more proper (with my bool.h) bool_cst.
//
// Revision 1.4  1996/08/24 13:01:39  hopper
// Added new operator ! method as a counterpart to the operator bool
// conversion.
//
// Revision 1.3  1996/07/07 20:57:27  hopper
// Fixed bug in StrChunkPtr::i_CheckType
//
// Revision 1.2  1996/07/05 19:46:25  hopper
// Various changed to make implementing StrChunkPtrT template easier and
// more efficient.
//
// Revision 1.1  1996/06/29 06:55:50  hopper
// New class StrChunkPtr that acts as a reference counted pointer to
// StrChunk
//

#include <assert.h>
#include <LCore/Protocol.h>
#include <LCore/RefCountPtrT.h>
#ifndef _STR_STR_ClassIdent_H_
#  include <StrMod/STR_ClassIdent.h>
#endif
#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif

#define _STR_StrChunkPtr_H_

class StrChunk;

class StrChunkPtr : public RefCountPtrT<StrChunk> {
 public:
   typedef RefCountPtrT<StrChunk> super1;
   static const STR_ClassIdent identifier;

   inline StrChunkPtr(const StrChunkPtr &b);
   inline StrChunkPtr(const super1 &b);
   inline StrChunkPtr(StrChunk *stptr = 0);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline const StrChunkPtr &operator =(const StrChunkPtr &b);
   inline const StrChunkPtr &operator =(const super1 &b);
   inline const StrChunkPtr &operator =(StrChunk *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline StrChunkPtr::StrChunkPtr(const StrChunkPtr &b) : super1(b)
{
}

inline StrChunkPtr::StrChunkPtr(const super1 &b) : super1(b)
{
}

inline StrChunkPtr::StrChunkPtr(StrChunk *stptr) : super1(stptr)
{
}

inline int StrChunkPtr::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline const StrChunkPtr &StrChunkPtr::operator =(const StrChunkPtr &b)
{
   //lint -save -e1529
   super1::operator =(b);
   return(*this);
   //lint -restore
}

inline const StrChunkPtr &StrChunkPtr::operator =(const super1 &b)
{
   super1::operator =(b);
   return(*this);
}

inline const StrChunkPtr &StrChunkPtr::operator =(StrChunk *b)
{
   super1::operator =(b);
   return(*this);
}

#endif
