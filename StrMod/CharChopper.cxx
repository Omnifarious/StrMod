/* $Header$ */

// For a log, see Changelog
//
// Revision 1.1  1996/09/02 23:28:22  hopper
// Added CharChopper class so users would have a simple class that would
// break up and recombine streams using whatever character they chose as
// a separator.
//

#ifdef __GNUG__
#  pragma implementation "CharChopper.h"
#endif

#include <StrMod/CharChopper.h>
#include <StrMod/StrChunk.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/GroupChunk.h>
#include <StrMod/GroupVector.h>
#include <StrMod/GV_Iterator.h>
#include <StrMod/StrSubChunk.h>
#include <algorithm>
#include <cassert>

const STR_ClassIdent CharChopper::identifier(26UL);

void CharChopper::addChunk(const StrChunkPtr &chnk)
{
   bool iscurdata = false;

   if (chnk.GetPtr() == curdata_.GetPtr())
   {
      curdata_->Resize(usedsize_);
      iscurdata = true;
   }
   else if (curdata_)
   {
      addChunk(curdata_);
   }
   if (outgoing_)
   {
      if (!groupdata_)
      {
	 if (outgoing_->AreYouA(GroupChunk::identifier))
	 {
	    groupdata_ = static_cast<GroupChunk *>(outgoing_.GetPtr());
	 }
	 else
	 {
	    groupdata_ = new GroupChunk;
	    groupdata_->push_back(outgoing_);
	    outgoing_ = groupdata_;
	 }
	 groupdata_->push_back(chnk);
      }
      else
      {
	 assert(outgoing_.GetPtr() == groupdata_.GetPtr());
	 groupdata_->push_back(chnk);
      }
   }
   else  // (!outgoing_)
   {
      if (groupdata_)
      {
	 groupdata_.ReleasePtr();
      }
      outgoing_ = chnk;
   }
   if (iscurdata)
   {
      curdata_.ReleasePtr();
   }
}

void CharChopper::processIncoming()
{
   assert(incoming_);
   assert(!outgoing_ready_);
   checkIncoming();
   assert(incoming_is_db_ != INMaybe);

   unsigned int inlen = incoming_->Length();

   if (inlen <= 0)
   {
      addChunk(incoming_);
      zeroIncoming();
   }
   else
   {
      char buf[16];
      size_t count = 0;
      bool foundchar = false;

      {
	 GV_Size gvsize = incoming_->NumSubGroups();
	 GroupVector gvec(gvsize);
	 incoming_->SimpleFillGroupVec(gvec);
	 for (GroupVector::iterator i = gvec.begin(); !foundchar && i; ++i)
	 {
	    if (count < 16)
	    {
	       buf[count] = *i;
	    }
	    count++;  // To include character when we find it.
	    if (*i == chopchar_)
	    {
	       foundchar = true;
	    }
	 }
      }
      assert((count >= inlen) || foundchar);
      if ((incoming_is_db_ == INYes) && (count <= 16))
      {
	 if (!curdata_)
	 {
	    if (foundchar)
	    {
	       curdata_ = new DataBlockStrChunk(count);
	    }
	    else
	    {
	       curdata_ = new DataBlockStrChunk(32);
	    }
	    memcpy(curdata_->GetVoidP(), buf, count);
	    usedsize_ = count;
	 }
	 else
	 {
	    unsigned int curlen = curdata_->Length();
	    size_t usedshadow = usedsize_;
	    if (curlen < (usedshadow + count))
	    {
	       if (foundchar)
	       {
		  curlen = usedshadow + count;
	       }
	       else
	       {
		  curlen = max(usedshadow + usedshadow / 2, usedshadow + count);
	       }
	       curdata_->Resize(curlen);
	    }
	    memcpy(curdata_->GetCharP() + usedshadow, buf, count);
	    usedsize_ = usedshadow = (usedshadow + count);
	 }
      }
      else // count > 16 || incoming_ isn't a DBStrChunk so buf can't be used.
      {
	 if (count >= inlen)
	 {
	    addChunk(incoming_);
	 }
	 else
	 {
	    addChunk(new StrSubChunk(incoming_, LinearExtent(0, count)));
	 }
      }
      if (count >= inlen)
      {
	 zeroIncoming();
      }
      else
      {
	 replaceIncoming(new StrSubChunk(incoming_,
					 LinearExtent(count, inlen)));
      }
      if (foundchar && curdata_)
      {
	 addChunk(curdata_);
      }
      outgoing_ready_ = foundchar;
   }
}
