/* $Header$ */

// $Log$
// Revision 1.1  1996/06/29 06:57:44  hopper
// New class StrSubChunk used to make a part of an existing chunk look like
// a full chunk to the outside world.
//

#ifdef __GNUG__
#  pragma implementation "StrSubChunk.h"
#endif

#include "StrMod/StrSubChunk.h"
#include <assert.h>
// #include <iostream.h>

const STR_ClassIdent StrSubChunk::identifier(18UL);

StrSubChunk::StrSubChunk(const StrChunkPtr &chunk, const LinearExtent &extent)
     : subchunk_(chunk), subext_(extent)
{
   assert(subchunk_);

   unsigned int sublen = subchunk_->Length();

   if ((subext_.Offset() + subext_.Length()) > sublen) {
      LinearExtent chunkext(0, sublen);

//      cerr << "chunkext == " << chunkext
//	   << " && subext_ == " << subext_ << "\n";
      subext_ = chunkext.SubExtent(subext_);
//      cerr << "subext_ == " << subext_ << "\n";
   }
}
