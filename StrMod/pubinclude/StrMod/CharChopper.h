#ifndef _STR_CharChopper_H_  // -*-c++-*-

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
#include <memory>
#include <StrMod/StreamProcessor.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/BufferChunk.h>
#include <StrMod/GroupChunk.h>

#define _STR_CharChopper_H_

namespace strmod {
namespace strmod {

class GroupVector;

/** \class CharChopper CharChopper.h StrMod/CharChopper.h
 * \brief Chops up the data in chunks delimited by a character.
 *
 * If an object of this class is constructed with a 'g', and you have
 * an incoming stream of data that arrives in the following chunks:
 *
 * \code <This ghost> < went to the gilded post t> <o admire the gold.> \endcode
 *
 * You'll end up with an outgoing stream of data that looks like this:
 *
 * \code <This g> <host went to the g> <ilded post to admire the g> \endcode
 *
 * It will hold the last few characters <old.>, waiting for another 'g' before
 * it send them off again.
 */
class CharChopper : public StreamProcessor
{
 public:
   /** Make a CharChopper
    * @param chopchar The character to split by.
    *
    * The character can be anything, including '\\0'.
    */
   CharChopper(char chopchar) : chopchar_(chopchar)    { }
   // Derived class destructor doesn't do anything base class one doesn't do.

 private:
   const char chopchar_;
   typedef ::std::shared_ptr<GroupChunk> groupptr_t;
   groupptr_t groupdata_;
   ::std::shared_ptr<BufferChunk> curdata_;
   size_t usedsize_;
   enum { INYes, INNo, INMaybe } incoming_is_bc_;

   void processIncoming() override;

   void addChunk(const StrChunkPtr &chnk);
   inline void checkIncoming();
   inline void zeroIncoming();
   inline void replaceIncoming(const StrChunkPtr &data);

 private:
   CharChopper(const CharChopper &b);
};

//-----------------------------inline functions--------------------------------

inline void CharChopper::checkIncoming()
{
   assert(incoming_);
   if (incoming_is_bc_ == INMaybe)
   {
      using ::std::dynamic_pointer_cast;

      if (dynamic_pointer_cast<BufferChunk, StrChunk>(incoming_))
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
   incoming_.reset();
   incoming_is_bc_ = INMaybe;
}

inline void CharChopper::replaceIncoming(const StrChunkPtr &data)
{
   incoming_ = data;
}

}  // namespace strmod
}  // namespace strmod

#endif
