#ifndef _STR_StrChunkPtr_H_  // -*-c++-*-

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

// For a change log see ../ChangeLog
// 
// Revision 1.5  1998/11/03 00:27:06  hopper
// Changed operator bool to the more proper (with my bool.h) bool_cst.
//
// Revision 1.4  1996/08/24 13:01:39  hopper
// Added new operator ! method as a counterpart to the operator bool
// conversion.
//
// Revision 1.3  1996/07/07 20:57:27  hopper
// Fixed bug in StrChunkPtr::i_CheckType
//
// Revision 1.2  1996/07/05 19:46:25  hopper
// Various changed to make implementing StrChunkPtrT template easier and
// more efficient.
//
// Revision 1.1  1996/06/29 06:55:50  hopper
// New class StrChunkPtr that acts as a reference counted pointer to
// StrChunk
//

#include <memory>

#define _STR_StrChunkPtr_H_

namespace strmod {
namespace strmod {

class StrChunk;

typedef ::std::shared_ptr<StrChunk> StrChunkPtr;

}  // namespace strmod
}  // namespace strmod

#endif
