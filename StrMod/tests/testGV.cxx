/* $Header$ */

// $Log$
// Revision 1.1  1996/06/29 07:01:08  hopper
// New test to work the GroupVector, and some of the other components of
// the new StrChunk style.
//

#include <StrMod/StrChunkPtr.h>
#include <StrMod/DBStrChunk.h>
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
      StrChunkPtr dbchnk = new DataBlockStrChunk(data[i].data, data[i].len);

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