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

// For a log, see ChangeLog
//
// Revision 1.3  1998/05/01 11:58:44  hopper
// Cleaned DelReference up a little.
//
// Revision 1.2  1997/05/12 16:34:53  hopper
// Fixed #include directives so auto-dependency generator would work better.
// --
// Also fixed reference count is decremented to 0 before actual deletion.
// This doesn't fix any current bug, but may prevent future ones in
// multi-threaded environments, and also reduces confusion.
//
// Revision 1.1  1997/05/12 14:32:39  hopper
// Added new RefCountPtr class, and RefCountPtrT class to aid in using
// the ReferenceCounting mixin class.
//

#ifdef __GNUG__
#  pragma implementation "RefCountPtr.h"
#endif

#define _LCORE_RefCountPtr_H_CC
#include "LCore/RefCountPtr.h"
#undef _LCORE_RefCountPtr_H_CC

#include "LCore/RefCounting.h"

namespace strmod {
namespace lcore {

const LCore_ClassIdent RefCountPtr::identifier(9UL);

void RefCountPtr::i_SetPtr(ReferenceCounting *p, bool deleteref)
{
   if (p) {
      p->AddReference();
   }
   if (ptr_ && deleteref) {
      if (ptr_->NumReferences() > 0) {
	    ptr_->DelReference();
      }
      if (ptr_->NumReferences() <= 0) {
	 delete ptr_;
      }
   }
   ptr_ = p;
}

} // namespace lcore
} // namespace strmod
