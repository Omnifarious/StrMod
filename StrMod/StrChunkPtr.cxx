/* $Header$ */

// For log see ChangeLog
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
