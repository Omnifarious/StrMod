/* $Header$ */

// $Log$
// Revision 1.2  1999/01/17 18:32:05  hopper
// Changed to automatically make a StrSubChunk of a StrSubChunk into just a
// StrSubChunk.
//
// Revision 1.1  1996/06/29 06:57:44  hopper
// New class StrSubChunk used to make a part of an existing chunk look like
// a full chunk to the outside world.
//

#ifdef __GNUG__
#  pragma implementation "StrSubChunk.h"
#endif

#include "StrMod/StrSubChunk.h"
#include "StrMod/StrChunkPtrT.h"
#include "StrMod/StrChunkPtr.h"
#include <assert.h>
// #include <iostream.h>

const STR_ClassIdent StrSubChunk::identifier(18UL);

StrSubChunk::StrSubChunk(const StrChunkPtr &chunk, const LinearExtent &extent)
     : subchunk_(chunk), subext_(extent)
{
   assert(subchunk_);

   if (subchunk_->AreYouA(identifier))
   {
      StrChunkPtrT<StrSubChunk> subc
	 = static_cast<StrSubChunk *>(subchunk_.GetPtr());

      subext_ = subc->subext_.SubExtent(subext_);
      subchunk_ = subc->subchunk_;
   }
   else
   {
      unsigned int sublen = subchunk_->Length();

      if ((subext_.Offset() + subext_.Length()) > sublen) {
	 LinearExtent chunkext(0, sublen);

//      cerr << "chunkext == " << chunkext
//	   << " && subext_ == " << subext_ << "\n";
	 subext_.SubExtent_eq(chunkext);
//      cerr << "subext_ == " << subext_ << "\n";
      }
   }
}
