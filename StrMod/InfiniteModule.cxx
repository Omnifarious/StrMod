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

// For a log, see ./ChangeLog

#ifdef __GNUG__
#  pragma implementation "InfiniteModule.h"
#endif

#include "StrMod/InfiniteModule.h"
#include "StrMod/PreAllocBuffer.h"
#include <cstring>  // memset
#include <memory>

namespace strmod {
namespace strmod {

InfiniteModule::InfiniteModule(const StrChunkPtr &chnk)
     : feed_(chnk), plug_created_(false), plug_(*this)
{
}

InfiniteModule::InfiniteModule()
     : plug_created_(false), plug_(*this)
{
   const int bufsize = 8192;
   ::std::shared_ptr<PreAllocBuffer<bufsize> > buf(
      new PreAllocBuffer<bufsize>
      );

   buf->resize(bufsize);
   memset(buf->getVoidP(), '\0', bufsize);
   feed_ = buf;
}

InfiniteModule::~InfiniteModule()
{
}

bool InfiniteModule::deletePlug(Plug *plug)
{
   if ((&plug_ == plug) && plug_created_)
   {
      plug_created_ = false;
      plug_.unPlug();
      setReadableFlagFor(&plug_, false);
      setWriteableFlagFor(&plug_, false);
      return(true);
   }
   else
   {
      return(false);
   }
}

StreamModule::Plug *InfiniteModule::i_MakePlug(int)
{
   plug_created_ = true;
   setReadableFlagFor(&plug_, true);
   setWriteableFlagFor(&plug_, true);
   return(&plug_);
}

}  // End namespace strmod
}  // End namespace strmod
