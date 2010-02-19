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

/* $Header$ */

// For log information, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "StreamModule.h"
#endif

#include "StrMod/StreamModule.h"
#include "StrMod/StrChunkPtr.h"

namespace strmod {
namespace strmod {

//! A unique identifier for this class.
const STR_ClassIdent StreamModule::identifier(1UL);
//! A unique identifier for this class.
const STR_ClassIdent StreamModule::Plug::identifier(2UL);

/*!
 * \param other A reference to the plug to plug into.  <strong>(A pointer to
 * it is stored)</strong>.
 *
 * A pointer to \a other will be stored by the plug until it's unplugged or
 * destroyed.  \a other is a reference because it's not allowed to be \c NULL.  */
bool StreamModule::Plug::plugInto(Plug &other)
{
   // Make sure neither plug is already plugged in.
   if ((other_ == NULL) && (other.other_ == NULL))
   {
      other_ = &other;
      other.other_ = this;
      flags_.notifyonread_ = other.needsNotifyReadable();
      other.flags_.notifyonread_ = needsNotifyReadable();
      flags_.notifyonwrite_ = other.needsNotifyWriteable();
      other.flags_.notifyonwrite_ = needsNotifyWriteable();
      notifyOtherReadable();
      if (other_ != NULL)
      {
	 other_->notifyOtherReadable();
      }
      notifyOtherWriteable();
      if (other_ != NULL)
      {
	 other_->notifyOtherWriteable();
      }
      if (isReadable() && (pluggedInto() != NULL))
      {
	 pushLoop();
      }
      if (isWriteable() && (pluggedInto() != NULL))
      {
	 pullLoop();
      }
      return(true);
   }
   else
   {
      return(false);
   }
}

/** \name Push and pulll loops
 * The loops that get everything done.
 *
 * Because of these loops, and the infrastructure that exists in the base
 * StreamModule and StreamModule::Plug class that calls them, you should never
 * have to call \c i_Read() or \c i_Write().
 *
 * These member functions are part of that infrastructure.  They handle
 * setting the \p iswriting_ and \p isreading_ flags properly.  They also loop
 * until no more data can be moved in the direction they're moving it because
 * one Plug became unwriteable, or one plug became unreadable.
 *
 * In short, don't bypass them unless you really know what you're doing.
 */
//@{
/*!
 * This keeps the isreading and iswriting flags set until the loop exits.
 *
 * This loop can exit for a number of reasons, including disconnection, one
 * side 'drying up' and having nothing to read, or one side 'filling up'
 * and not being able to be written to.
 */
void StreamModule::Plug::pushLoop()
{
   assert(isReadable() && (flags_.isreading_ == false));

   Plug *other = pluggedInto();

   assert(other != NULL);

   if (other != NULL && other->isWriteable())
   {
      other->setIsWriting(true);
   }
   setIsReading(true);
   while (flags_.canread_ && (other != NULL) && getFlagsFrom(*other).canwrite_)
   {
      other->i_Write(i_Read());

      if (pluggedInto() != other)
      {
	 other->setIsWriting(false);
	 other = pluggedInto();
	 if (other != NULL && other->isWriteable())
	 {
	    other->setIsWriting(true);
	 }
      }
   }
   setIsReading(false);
   if (other != NULL)
   {
      other->setIsWriting(false);
   }
}

/*!
 * This keeps the isreading and iswriting flags set until the loop exits.
 *
 * This loop can exit for a number of reasons, including disconnection, one
 * side 'drying up' and having nothing to read, or one side 'filling up' and
 * not being able to be written to.
 */
void StreamModule::Plug::pullLoop()
{
   assert(isWriteable() && (flags_.iswriting_ == false));

   Plug *other = pluggedInto();

   assert(other != NULL);

   setIsWriting(true);
   if (other != NULL && other->isReadable())
   {
      other->setIsReading(true);
   }
   while (flags_.canwrite_ && (other != NULL) && getFlagsFrom(*other).canread_)
   {
      i_Write(other->i_Read());

      if (pluggedInto() != other)
      {
	 other->setIsReading(false);
	 other = pluggedInto();
	 if (other != NULL && other->isReadable())
	 {
	    other->setIsReading(true);
	 }
      }
   }
   setIsWriting(false);
   if (other != NULL)
   {
      other->setIsReading(false);
   }
}
//@}

}  // End namespace strmod
}  // End namespace strmod
