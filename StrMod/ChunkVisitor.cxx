/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "ChunkVisitor.h"
#endif

#include "StrMod/ChunkVisitor.h"
#include "StrMod/StrChunk.h"
#include "StrMod/StrChunkPtr.h"

const STR_ClassIdent ChunkVisitor::identifier(7UL);

void ChunkVisitor::call_acceptVisitor(const StrChunkPtr &chnk)
   throw(halt_visitation)
{
   chnk->acceptVisitor(*this);
}
