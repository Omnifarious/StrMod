#ifndef _STR_StrChunk_H_  // -*-c++-*-

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.

#include <cassert>

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

//: A reference counted chunk of data.
// Use the StrChunkPtr class if you want the reference count to be maintained
// automatically.
class StrChunk : public Object, public ReferenceCounting {
 public:
   //: Describes how 0 length chunks should be handled when chunks are dropped.
   // <dl><dt>KeepLeft</dt> <dd>Drop all trailing 0 length chunks and keep all
   // leading 0 length chunks.</dd>
   // <dt>KeepRight</dt> <dd>Dropp all leading 0 length chunks and keep all
   // trailing 0 length chunks.</dd>
   // <dt>KeepNone</dt> <dd>Drop both leading and trailing 0 length
   // chunks.</dd></dl>
   enum KeepDir { KeepLeft, KeepRight, KeepNone };
   static const STR_ClassIdent identifier;

   StrChunk() : ReferenceCounting(0)         { }
   virtual ~StrChunk()                       { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: Number of octets would this chunk takes up.  May be deprecated.
   virtual unsigned int Length() const = 0;
   //: Number of 'sub groups' make up this entire chunk?
   virtual unsigned int NumSubGroups() const;
   //: How many 'sub groups' make up the section of this chunk denoted by
   //: extent.
   virtual unsigned int NumSubGroups(const LinearExtent &extent) const = 0;
   //: Fill a GroupVector with a list of all groups making up this chunk.
   inline void SimpleFillGroupVec(GroupVector &vec);
   //: Fill a GroupVector with a list of all groups making up this chunk.
   // Fill a GroupVector with a list of all groups making up this chunk
   // starting at index start_index in the GroupVector.
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   //: Fill a GroupVector with a list of groups in this chunk.
   // Fill a GroupVector with a list of groups in this chunk falling within
   // the section denoted by extent, starting at index start_index in the
   // GroupVector.
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index) = 0;
   //: Drop any unused sub chunks from this StrChunk.  <strong>Largely
   //: unimplemented and unsafe to use.  Interface may change.</strong>
   // This is a dummy function that checks the reference count first, then
   // calls i_DropUnused.  If this chunk is referenced from more than one
   // place a determination of which chunks are unused is impossible.
   inline void DropUnused(const LinearExtent &usedextent,
			  KeepDir keepdir = KeepLeft);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //: The actual DropUnused function that needs to be overloaded in derived
   //: classes.
   // This only really has meaning for compound StrChunks.  The keepdir
   // parameter tells the function how to handle 0 length StrChunks.  See the
   // documentation for KeepDir for more information.
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
