/* $Header$ */

// See ChangeLog for log.
// Revision 1.1  1996/06/29 06:51:22  hopper
// New class GroupChunk to hold a group of chunks and make them appear to
// be one chunk.
//

#ifdef __GNUG__
#  pragma implementation "GroupChunk.h"
#endif

#include <StrMod/GroupChunk.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/StrChunk.h>
#include <StrMod/LinearExtent.h>
// #include <iostream.h>

const STR_ClassIdent GroupChunk::identifier(20UL);

// We deal with StrChunk *'s directly because it's more efficient.  We try
// to hide this fact as much as possible from the outside world.

GroupChunk::GroupChunk() : totalsize_(0)
{
}

GroupChunk::~GroupChunk()
{
   ChunkList::iterator i;
   ChunkList::iterator end;

   end = chnklist_.end();
   for (i = chnklist_.begin(); i != end; ++i) {
      StrChunk *ptr = *i;

      *i = 0;
      if (ptr) {
	 ptr->DelReference();
	 if (ptr->NumReferences() <= 0) {
	    delete ptr;
	 }
      }
   }
}

unsigned int GroupChunk::NumSubGroups() const
{
   ChunkList::const_iterator i;
   ChunkList::const_iterator end;
   unsigned int groups;

   groups = 0;
   end = chnklist_.end();
   for (i = chnklist_.begin(); i != end; ++i) {
      StrChunk *ptr = *i;

      if (ptr && (ptr->Length() > 0)) {
	 groups += ptr->NumSubGroups();
      }
   }
   return(groups);
}

unsigned int GroupChunk::NumSubGroups(const LinearExtent &extent) const
{
//   cerr << "GroupChunk::NumSubGroups\n";
//   cerr << "extent == " << extent << "\n";
   if ((extent.Offset() == 0) && (extent.Length() >= totalsize_)) {
      return(NumSubGroups());
   } else {
      NumGroupAccumulator accum;

      AccumulateRange(extent, accum);
//      cerr << "Returning " << accum.NumGroups() << "\n";
      return(accum.NumGroups());
   }
}

void GroupChunk::FillGroupVec(GroupVector &vec, unsigned int &start_index)
{
   ChunkList::iterator i;
   ChunkList::iterator end;

   end = chnklist_.end();
   for (i = chnklist_.begin(); i != end; ++i) {
      StrChunk *ptr = *i;

      if (ptr && (ptr->Length() > 0)) {
	 ptr->FillGroupVec(vec, start_index);
      }
   }
}

void GroupChunk::FillGroupVec(const LinearExtent &extent,
			      GroupVector &vec, unsigned int &start_index)
{
//   cerr << "GroupChunk::NumSubGroups\n";
//   cerr << "extent == " << extent << "\n";
//   cerr << "start_index == " << start_index << "\n";
   if ((extent.Offset() == 0) && (extent.Length() >= totalsize_)) {
      FillGroupVec(vec, start_index);
   } else {
      GroupVecAccumulator accum(&vec, &start_index);

      AccumulateRange(extent, accum);
   }
}

void GroupChunk::push_back(const StrChunkPtr &chnk)
{
   StrChunk *ptr = chnk.GetPtr();

   ptr->AddReference();
   chnklist_.push_back(ptr);
   totalsize_ += ptr->Length();
}

void GroupChunk::push_front(const StrChunkPtr &chnk)
{
   StrChunk *ptr = chnk.GetPtr();

   ptr->AddReference();
   chnklist_.push_front(ptr);
   totalsize_ += ptr->Length();
}

static inline bool lower_compare(const StrChunk::KeepDir &keepdir,
				 const LinearExtent::off_t &offset,
				 const unsigned int &ptrlen)
{
   switch (keepdir) {
    case StrChunk::KeepLeft:
    default:
      return(offset > ptrlen);
    case StrChunk::KeepRight:
      return(offset >= ptrlen);
    case StrChunk::KeepNone:
      return(offset >= ptrlen);
   }
}

static inline bool upper_compare(const StrChunk::KeepDir &keepdir,
				 const unsigned int &totalsize,
				 const unsigned int &ptrlen,
				 const LinearExtent::length_t &length)
{
   switch (keepdir) {
    case StrChunk::KeepLeft:
    default:
      return((totalsize - ptrlen) >= length);
    case StrChunk::KeepRight:
      return((totalsize - ptrlen) > length);
    case StrChunk::KeepNone:
      return((totalsize - ptrlen) >= length);
   }
}

void GroupChunk::i_DropUnused(const LinearExtent &usedextent, KeepDir keepdir)
{
   assert((keepdir == KeepLeft) ||
	  (keepdir == KeepRight) || (keepdir = KeepNone));

   if ((usedextent.Offset() == 0) && (usedextent.Length() > totalsize_)) {
      return;
   } else if ((keepdir != KeepNone) && (totalsize_ == 0)) {
      return;
   } else {
      LinearExtent ext = usedextent;

      while (chnklist_.size() > 0) {
	 StrChunk *ptr = chnklist_.front();

	 if (ptr) {
	    unsigned int ptrlen = ptr->Length();

	    assert(totalsize_ >= ptrlen);
	    if (lower_compare(keepdir, ext.Offset(), ptrlen)) {
	       chnklist_.pop_front();
	       ptr->DelReference();
	       if (ptr->NumReferences() <= 0) {
		  delete ptr;
		  ptr = 0;
	       }
	       totalsize_ -= ptrlen;
	       ext.MoveLeft(ptrlen);
	    } else {
	       break;
	    }
	 }
      }

      ext.LengthenLeft(ext.Offset());

      while (chnklist_.size() > 0) {
	 StrChunk *ptr = chnklist_.back();

	 if (ptr) {
	    unsigned int ptrlen = ptr->Length();

	    assert(totalsize_ >= ptrlen);
	    if (upper_compare(keepdir, totalsize_, ptrlen, ext.Length())) {
	       chnklist_.pop_back();
	       ptr->DelReference();
	       if (ptr->NumReferences() <= 0) {
		  delete ptr;
		  ptr = 0;
	       }
	       totalsize_ -= ptrlen;
	    } else {
	       break;
	    }
	 }
      }
   }
}

void GroupChunk::AccumulateRange(const LinearExtent &extent,
				 RangeAccumulator &accum) const
{
   LinearExtent ext = extent;

   // >= because zero size chunks aren't allowed to have groups.

   if (ext.Offset() >= totalsize_) {
      return;
   } else if (ext.Length() == 0) {
      return;
   }

   ChunkList::const_iterator i;
   ChunkList::const_iterator end;
   StrChunk *ptr = 0;
   unsigned int ptrlen = 0;

   i = chnklist_.begin();
   end = chnklist_.end();
   // >= to skip zero size chunks since they can't have groups.
   while ((ext.Offset() >= 0) && (i != end)) {
      ptr = *i;
      if (ptr) {
	 ptrlen = ptr->Length();
	 if (ptrlen > ext.Offset()) {
	    break;
	 } else {
	    ext.MoveLeft(ptrlen);
	 }
      }
      ++i;
   }

   // The cases take care of at the very beginning should make it
   // impossible for this case to happen.
   assert(i != end);

   // Sometimes, the impossible happens (especially if NDEBUG is set).
   if (i != end) {
      assert(ptr != 0);
      accum(*ptr, ext);
      ext.ShortenRight(ptrlen - ext.Offset());
      ext.Offset(0);
      ++i;

      while ((i != end) && (ext.Length() > 0)) {
	 ptr = *i;

	 if (ptr) {
	    ptrlen = ptr->Length();
	    // Skip zero size chunks since they can't have groups anyway.
	    if (ptrlen > 0) {
	       accum(*ptr, ext);
	       ext.ShortenRight(ptrlen);
	    }
	 }
	 ++i;
      }
   }
}

void GroupChunk::NumGroupAccumulator::operator ()(StrChunk &chnk,
						  const LinearExtent &ext)
{
   numgroups_ += chnk.NumSubGroups(ext);
}

void GroupChunk::GroupVecAccumulator::operator ()(StrChunk &chnk,
						  const LinearExtent &ext)
{
   chnk.FillGroupVec(ext, vec_, index_);
}
