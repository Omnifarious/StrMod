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

// See ChangeLog for log.
// Revision 1.4  1996/12/10 01:53:59  hopper
// Fixed an error in FillIOVecDesc involving a misplaced increment.
//
// Revision 1.3  1996/09/11 23:10:34  hopper
// Changed class GroupVector to be a little more STL like.  Added
// SafeToDelete method.
//
// Revision 1.2  1996/06/29 06:39:54  hopper
// Added initialization for totallength_
//
// Revision 1.1  1996/05/08 10:56:22  hopper
// Initial functional revision
//

#ifdef __GNUG__
#  pragma implementation "GroupVector.h"
#endif

#include <StrMod/GroupVector.h>

const STR_ClassIdent GroupVector::identifier(16UL);

GroupVector::GroupVector(GV_Size size)
     : groups_(0), numgroups_(size), totallength_(0), fakegroup_(false),
       lastoffset_(0), lastgroupnum_(0), numiterators_(0)
{
   if (numgroups_ > 0) {
      groups_ = new iovec[numgroups_];
      for (GV_Size i = 0; i < numgroups_; i++) {
	 groups_[i].iov_base = 0;
	 groups_[i].iov_len = 0;
      }
   }
}

GroupVector::~GroupVector()
{
   assert(SafeToDelete());

   if (numgroups_ > 0) {
      delete[] groups_;
   }
}

bool GroupVector::FillIOVecDesc(size_t offset, GroupVector::IOVecDesc &desc)
{
   assert(numiterators_ <= 0);

   if (fakegroup_) {
      fakegroup_ = false;
      groups_[fakegroupnum_] = realgroupdata_;
      fakegroupnum_ = 0;
   }

   if (offset == 0) {
      desc.iov = &(groups_[0]);
      desc.iovcnt = numgroups_;
      lastoffset_ = 0;
      lastgroupnum_ = 0;
      return(true);
   } else {
      GV_Size group_num = 0;
      size_t current_off = 0;

      if ((lastoffset_ > 0) && (offset > lastoffset_)) {
	 group_num = lastgroupnum_;
	 current_off = lastoffset_;
      }
      while ((group_num < numgroups_)
	     && (offset >= (current_off + groups_[group_num].iov_len))) {
	 current_off += groups_[group_num].iov_len;
	 ++group_num;
      }
      if (group_num < numgroups_) {
	 assert(current_off <= offset);

	 lastoffset_ = current_off;
	 lastgroupnum_ = group_num;
	 desc.iov = &(groups_[group_num]);
	 desc.iovcnt = numgroups_ - group_num;

	 if (offset > current_off) {
	    size_t off_adj = offset - current_off;

	    assert(off_adj < groups_[group_num].iov_len);

	    fakegroup_ = true;
	    fakegroupnum_ = group_num;
	    realgroupdata_ = groups_[group_num];

	    const char *tmp =
	       static_cast<char *>(groups_[group_num].iov_base);

	    groups_[group_num].iov_base = const_cast<char *>(tmp + off_adj);
	    groups_[group_num].iov_len -= off_adj;
	 }
	 return(true);
      } else {
	 return(false);
      }
   }
}

void GroupVector::FreeAllIOVecDescs()
{
   lastoffset_ = 0;
   lastgroupnum_ = 0;
   if (fakegroup_) {
      groups_[fakegroupnum_] = realgroupdata_;
      fakegroup_ = false;
      fakegroupnum_ = 0;
   }
}
