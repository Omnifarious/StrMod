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

#ifdef __GNUG__
#pragma implementation "EchoModule.h"
#endif

// For log information, see ChangeLog

// $Revision$


#include "StrMod/EchoModule.h"

namespace strmod {
namespace strmod {

const STR_ClassIdent EchoModule::identifier(3UL);
const STR_ClassIdent EchoModule::EPlug::identifier(4UL);

EchoModule::EchoModule() : plugcreated_(false), eplug_(*this)
{
}

EchoModule::~EchoModule()
{
}

bool EchoModule::deletePlug(Plug *plug)
{
   if (i_OwnsPlug(plug))
   {
      plugcreated_ = false;
      plug->unPlug();
      return(true);
   }
   else
   {
      return(false);
   }
}

void EchoModule::plugDisconnected(Plug *plug)
{
   assert(plug == &eplug_);
   assert(plugcreated_);

   setReadableFlagFor(&eplug_, false);
   setWriteableFlagFor(&eplug_, false);
   StreamModule::plugDisconnected(plug);
}

void EchoModule::EPlug::otherIsReadable()
{
   Plug *other = pluggedInto();

   setReadable((other == NULL) ? false : getFlagsFrom(*other).canread_);
}

void EchoModule::EPlug::otherIsWriteable()
{
   Plug *other = pluggedInto();

   setWriteable((other == NULL) ? false : getFlagsFrom(*other).canwrite_);
}

const StrChunkPtr EchoModule::EPlug::i_Read()
{
   StrChunkPtr tmp = readOther();
   Plug *other = pluggedInto();

   if (other != NULL)
   {
      setReadable(getFlagsFrom(*other).canread_);
   }
   else
   {
      setReadable(false);
      setWriteable(false);
   }
   return(tmp);
}

void EchoModule::EPlug::i_Write(const StrChunkPtr &ptr)
{
   writeOther(ptr);

   Plug *other = pluggedInto();

   if (other != NULL)
   {
      setWriteable(getFlagsFrom(*other).canwrite_);
   }
   else
   {
      setReadable(false);
      setWriteable(false);
   }
}

};  // End namespace strmod
};  // End namespace strmod
