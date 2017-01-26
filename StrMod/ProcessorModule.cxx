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

// For a log, see ChangeLog
//
// Revision 1.2  1996/09/03 01:58:25  hopper
// Commented out all kinds of debugging stuff.
//
// Revision 1.1  1996/09/02 23:31:12  hopper
// Added class that would turn two StreamProcessors into a StreamModule
// that used one StreamProcessor for the data flowing in each direction.
//

#ifdef __GNUG__
#  pragma implementation "ProcessorModule.h"
#endif

#include "StrMod/ProcessorModule.h"
#include "StrMod/StreamProcessor.h"
#include "StrMod/StrChunkPtr.h"
#include <cassert>
// #include <iostream>

namespace strmod {
namespace strmod {

bool ProcessorModule::deletePlug(Plug *plug)
{
   if ((plug == &side_) && pulled_.side)
   {
      side_.unPlug();
      pulled_.side = false;
      return(true);
   }
   else if (plug == &otherside_ && pulled_.otherside)
   {
      otherside_.unPlug();
      pulled_.otherside = false;
      return(true);
   }
   return(false);
}

ProcessorModule::PMPlug::~PMPlug()
{
   if (getParent().pulled_.owns)
   {
      delete &readproc_;
   }
}

const StrChunkPtr ProcessorModule::PMPlug::i_Read()
{
   assert(getFlagsFrom(*this).canread_);
   assert(readproc_.canReadFrom());
   StrChunkPtr tmp = readproc_.readFrom();
   getParent().setWriteableFlagFor(&sibling_, readproc_.canWriteTo());
   setReadable(readproc_.canReadFrom());
   return(tmp);
}

void ProcessorModule::PMPlug::i_Write(const StrChunkPtr &chnk)
{
   assert(getFlagsFrom(*this).canwrite_);
   assert(writeproc_.canWriteTo());
   writeproc_.writeTo(chnk);
   getParent().setReadableFlagFor(&sibling_, writeproc_.canReadFrom());
   setWriteable(writeproc_.canWriteTo());
}

}  // End namespace strmod
}  // End namespace strmod
