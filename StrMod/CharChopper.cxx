/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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
#include <StrMod/ChunkIterator.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/GroupChunk.h>
#include <StrMod/StrSubChunk.h>
#include <StrMod/PreAllocBuffer.h>
#include <StrMod/DynamicBuffer.h>
#include <algorithm>
#include <cassert>
#include <cstring>

namespace strmod {
namespace strmod {

void CharChopper::addChunk(const StrChunkPtr &chnk)
{
   bool iscurdata = false;

   if (chnk == curdata_)
   {
      curdata_->resize(usedsize_);
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
         using ::std::tr1::dynamic_pointer_cast;
	 if (groupptr_t tmp =
             dynamic_pointer_cast<GroupChunk, StrChunk>(outgoing_))
	 {
	    groupdata_.swap(tmp);
	 }
	 else
	 {
	    groupdata_.reset(new GroupChunk);
	    groupdata_->push_back(outgoing_);
	    outgoing_ = groupdata_;
	 }
	 groupdata_->push_back(chnk);
      }
      else
      {
	 assert(outgoing_ == groupdata_);
	 groupdata_->push_back(chnk);
      }
   }
   else  // (!outgoing_)
   {
      if (groupdata_)
      {
	 groupdata_.reset();
      }
      outgoing_ = chnk;
   }
   if (iscurdata)
   {
      curdata_.reset();
   }
}

void CharChopper::processIncoming()
{
   assert(incoming_);
   assert(!outgoing_ready_);
   checkIncoming();
   assert(incoming_is_bc_ != INMaybe);

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
         StrChunk::const_iterator end = incoming_->end();
	 for (StrChunk::const_iterator i = incoming_->begin();
              !foundchar && i != end; ++i)
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
      if ((incoming_is_bc_ == INYes) && (count <= 16))
      {
	 if (!curdata_)
	 {
	    if (foundchar)
	    {
	       curdata_.reset(new PreAllocBuffer<16>);
	       curdata_->resize(count);
	    }
	    else
	    {
	       curdata_.reset(new DynamicBuffer(32));
	    }
	    ::std::memcpy(curdata_->getVoidP(), buf, count);
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
		  curlen = std::max(usedshadow + usedshadow / 2, usedshadow + count);
	       }
	       curdata_->resize(curlen);
	    }
	    ::std::memcpy(curdata_->getCharP() + usedshadow, buf, count);
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
	    addChunk(StrChunkPtr(new StrSubChunk(incoming_,
                                                 LinearExtent(0, count))));
	 }
      }
      if (count >= inlen)
      {
	 zeroIncoming();
      }
      else
      {
	 replaceIncoming(StrChunkPtr(new StrSubChunk(incoming_,
                                                     LinearExtent(count, inlen))));
      }
      if (foundchar && curdata_)
      {
	 addChunk(curdata_);
      }
      outgoing_ready_ = foundchar;
   }
}

};  // End namespace strmod
};  // End namespace strmod
