/* $Header$ */

// See ./ChangeLog for changes.

#ifdef __GNUG__
#  pragma implementation "StrChunk.h"
#endif

#include "StrMod/StrChunk.h"
#include "StrMod/LinearExtent.h"

const STR_ClassIdent StrChunk::identifier(6UL);

unsigned int StrChunk::NumSubGroups() const
{
   return(NumSubGroups(LinearExtent::full_extent));
}

void StrChunk::FillGroupVec(GroupVector &vec, unsigned int &start_index)
{
   FillGroupVec(LinearExtent::full_extent, vec, start_index);
}
