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

#ifndef NO_RcsID
static char _EH_StreamModule_CC_rcsID[] = "$Id$";
#endif

#ifdef __GNUG__
#  pragma implementation "StreamModule.h"
#endif

#include "StrMod/StreamModule.h"
#include "StrMod/StrChunkPtr.h"

const STR_ClassIdent StreamModule::identifier(1UL);
const STR_ClassIdent StreamModule::Plug::identifier(2UL);

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
