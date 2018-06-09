/*
 * Copyright 2001-2010 Eric M. Hopper <hopper@omnifarious.org>
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

#ifdef __GNUG__
#  pragma implementation "ApplyVisitor.h"
#endif

#include "StrMod/ApplyVisitor.h"
#include "StrMod/StrSubChunk.h"
#include "StrMod/StrChunkPtr.h"

namespace {

template <class T>
class stack_deleter {
 public:
   void operator()(T *) { }
};

}

namespace strmod {
namespace strmod {

ApplyVisitor_Base::ApplyVisitor_Base(const StrChunkPtr &chunk)
     : UseTrackingVisitor(true), chunk_(chunk), extent_used_(false)
{
}

ApplyVisitor_Base::ApplyVisitor_Base(const StrChunkPtr &chunk,
                                     LinearExtent &extent)
     : UseTrackingVisitor(true), chunk_(chunk),
       extent_used_(true), extent_(extent)
{
}

void ApplyVisitor_Base::apply()
{
   if (extent_used_)
   {
      StrSubChunk mychunk(chunk_, extent_);
      startVisit(StrChunkPtr(&mychunk, stack_deleter<StrChunk>()));
   }
   else
   {
      startVisit(chunk_);
   }
}

}  // End namespace strmod
}  // End namespace strmod
