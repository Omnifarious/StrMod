#ifndef _STR_StrChunk_H_  // -*-c++-*-

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.

#include <assert.h>

#ifndef _STR_STR_ClassIdent_H_
#  ifndef OS2
#     include <StrMod/STR_ClassIdent.h>
#  else
#     include "str_clas.h"
#  endif
#endif

#include <LCore/Object.h>
#include <LCore/RefCounting.h>

#define _STR_StrChunk_H_

class LinearExtent;
class GroupVector;

//------------------------------class StrChunk---------------------------------

class StrChunk : public Object, public ReferenceCounting {
 public:
   enum KeepDir { KeepLeft, KeepRight, KeepNone };
   static const STR_ClassIdent identifier;

   StrChunk() : ReferenceCounting(0)         { }
   virtual ~StrChunk()                       { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const = 0;
   virtual unsigned int NumSubGroups() const;
   virtual unsigned int NumSubGroups(const LinearExtent &extent) const = 0;
   inline void SimpleFillGroupVec(GroupVector &vec);
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index) = 0;
   inline void DropUnused(const LinearExtent &usedextent,
			  KeepDir keepdir = KeepLeft);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }


   virtual void i_DropUnused(const LinearExtent &usedextent,
			     KeepDir keepdir) = 0;
};

//------------------------inline functions for StrChunk------------------------

inline int StrChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

inline void StrChunk::SimpleFillGroupVec(GroupVector &vec)
{
   unsigned int i = 0;

   FillGroupVec(vec, i);
}

inline void StrChunk::DropUnused(const LinearExtent &usedextent,
				 KeepDir keepdir)
{
   if (NumReferences() <= 1) {
      i_DropUnused(usedextent, keepdir);
   }
}

#endif
