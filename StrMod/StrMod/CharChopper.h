#ifndef _STR_CharChopper_H_  // -*-c++-*-

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

// For a log, see ../Changelog
//
// Revision 1.1  1996/09/02 23:28:27  hopper
// Added CharChopper class so users would have a simple class that would
// break up and recombine streams using whatever character they chose as
// a separator.
//

#include <cstddef>
#include <cassert>
#include <StrMod/StreamProcessor.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/BufferChunk.h>
#include <StrMod/GroupChunk.h>

#define _STR_CharChopper_H_

namespace strmod {
namespace strmod {

class GroupVector;

class CharChopper : public StreamProcessor {
 public:
   static const STR_ClassIdent identifier;

   CharChopper(char chopchar) : chopchar_(chopchar)    { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   const char chopchar_;
   StrChunkPtrT<GroupChunk> groupdata_;
   StrChunkPtrT<BufferChunk> curdata_;
   size_t usedsize_;
   enum { INYes, INNo, INMaybe } incoming_is_bc_;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual void processIncoming();

   void addChunk(const StrChunkPtr &chnk);
   inline void checkIncoming();
   inline void zeroIncoming();
   inline void replaceIncoming(const StrChunkPtr &data);

 private:
   CharChopper(const CharChopper &b);
};

//-----------------------------inline functions--------------------------------

inline int CharChopper::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

inline void CharChopper::checkIncoming()
{
   assert(incoming_);
   if (incoming_is_bc_ == INMaybe)
   {
      if (incoming_->AreYouA(BufferChunk::identifier))
      {
	 incoming_is_bc_ = INYes;
      }
      else
      {
	 incoming_is_bc_ = INNo;
      }
   }
}

inline void CharChopper::zeroIncoming()
{
   incoming_.ReleasePtr();
   incoming_is_bc_ = INMaybe;
}

inline void CharChopper::replaceIncoming(const StrChunkPtr &data)
{
   incoming_ = data;
}

};  // namespace strmod
};  // namespace strmod

#endif
