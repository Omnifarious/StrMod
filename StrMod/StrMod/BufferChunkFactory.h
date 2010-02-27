#ifndef _STR_BufferChunkFactory_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog
//
// Revision 1.1  1999/09/16 01:41:21  hopper
// An interface for things that create BufferChunks.
//

#include <StrMod/BufferChunk.h>

#define _STR_BufferChunkFactory_H_

namespace strmod {
namespace strmod {

class BufferChunk::Factory
{
 public:
   Factory()                                            { }
   virtual ~Factory()                                   { }

   virtual BufferChunk *makeChunk() = 0;
};

};  // namespace strmod
};  // namespace strmod

#endif
