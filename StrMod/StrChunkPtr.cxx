/* $Header$ */

// $Log$
// Revision 1.3  1998/12/30 20:31:51  hopper
// Changed the whole face of the StrChunkPtr class to be derived from
// RefCountPtrT<StrChunk> because with current compiler has rendered this a
// safe thing to do, and it more accurately reflects semantics anyway.  As
// part of this, StrChunk has also been changed to derive from
// ReferenceCounting, because that is also more consistent with semantics,
// and allows the previously mentioned change.
//
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

#include <StrMod/StrChunkPtr.h>

const STR_ClassIdent StrChunkPtr::identifier(19UL);
