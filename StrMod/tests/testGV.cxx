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

// For a log, see ../ChangeLog
//
// Revision 1.2  1999/09/21 03:37:55  hopper
// Changed to use DynamicBuffer instead of DataBlockStrChunk.
//
// Revision 1.1  1996/06/29 07:01:08  hopper
// New test to work the GroupVector, and some of the other components of
// the new StrChunk style.
//

#include <StrMod/StrChunkPtr.h>
#include <StrMod/DynamicBuffer.h>
#include <StrMod/GroupChunk.h>
#include <StrMod/StrSubChunk.h>
#include <StrMod/GroupVector.h>
#include <StrMod/GV_Iterator.h>
#include <unistd.h>
#include <iostream.h>

struct testdata {
   char *data;
   size_t len;
};

static const int num_els = 4;

static testdata data[num_els] = {
   { "George ", sizeof("George ") - 1 },
   { "Orwell h", sizeof("Orwell h") - 1 },
   { "as fl", sizeof("as fl") - 1 },
   { "eas.", sizeof("eas.") - 1 }
};

static void FillGroupChunk(GroupChunk &gc)
{
   for (int i = 0; i < num_els; i++) {
      StrChunkPtr dbchnk = new DynamicBuffer(data[i].data, data[i].len);

      gc.push_back(dbchnk);
   }
}

int main()
{
   GroupChunk *gc = new GroupChunk;
   StrChunkPtr gcptr = gc;

   FillGroupChunk(*gc);

   StrChunkPtr sub1 = new StrSubChunk(gcptr, LinearExtent(0, 1));
   StrChunkPtr sub2 = new StrSubChunk(gcptr, LinearExtent(1, 13));
   StrChunkPtr sub3 =
      new StrSubChunk(gcptr, LinearExtent(14, gcptr->Length()));
   gc = new GroupChunk;
   StrChunkPtr subptr = gc;

   gc->push_back(sub1);
   gc->push_back(sub2);
   gc->push_back(sub3);
   GroupVector vec(subptr->NumSubGroups());
   subptr->SimpleFillGroupVec(vec);

   cout << "Iterator: \"";
   for (GroupVector::Iterator i = vec.begin(); i; ++i) {
      cout << *i;
   }
   cout << "\"\n";

   cout << "  writev: \"";
   cout.flush();
   GroupVector::IOVecDesc desc;

   vec.FillIOVecDesc(0, desc);
   writev(1, desc.iov, desc.iovcnt);
   cout << "\"\n";
   cout << "desc.iovcnt == " << desc.iovcnt << "\n";
   return(0);
}
