/* $Header$ */

// $Log$
// Revision 1.2  1996/07/05 19:46:33  hopper
// Various changed to make implementing StrChunkPtrT template easier and
// more efficient.
//
// Revision 1.1  1996/06/29 06:55:43  hopper
// New class StrChunkPtr that acts as a reference counted pointer to
// StrChunk
//

#ifdef __GNUG__
#  pragma implementation "StrChunkPtr.h"
#endif

#define CHUNK_PTR_CC
#include <StrMod/StrChunkPtr.h>
#undef CHUNK_PTR_CC

#include <StrMod/StrChunk.h>

const STR_ClassIdent StrChunkPtr::identifier(19UL);

void StrChunkPtr::i_SetPtr(StrChunk *p, bool deleteref)
{
   if (p) {
      p->AddReference();
   }
   if (ptr && deleteref) {
      if (ptr->NumReferences() > 0) {
	 ptr->DelReference();
      }
      if (ptr->NumReferences() == 0) {
	 delete ptr;
      }
   }
   ptr = p;
}
