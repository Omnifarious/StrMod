/*
 * Copyright 2000 by Eric M. Hopper <hopper@omnifarious.mn.org>
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
#  pragma implementation "ChunkVisitor.h"
#endif

#include "StrMod/ChunkVisitor.h"
#include "StrMod/StrChunk.h"
#include "StrMod/StrChunkPtr.h"

namespace strmod {
namespace strmod {

const STR_ClassIdent ChunkVisitor::identifier(7UL);

void ChunkVisitor::call_acceptVisitor(const StrChunkPtr &chnk)
   throw(halt_visitation)
{
   chnk->acceptVisitor(*this);
}

};  // End namespace strmod
};  // End namespace strmod
