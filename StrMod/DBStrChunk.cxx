/* $Header$ */

// $Log$
// Revision 1.2  1998/12/18 22:56:24  hopper
// Removed a couple of lines that were used for debugging.
//
// Revision 1.1  1996/06/29 06:49:13  hopper
// Moved from StrChunk.h and StrChunk.cc along with re-working for new
// StrChunk handling.
//

#ifdef __GNUG__
#  pragma implementation "DBStrChunk.h"
#endif

#include <stdlib.h>
#include "StrMod/DBStrChunk.h"
#include "StrMod/GroupVector.h"

const STR_ClassIdent DataBlockStrChunk::identifier(7UL);

DataBlockStrChunk::~DataBlockStrChunk()
{
   if (buf_ != 0) {
      free(buf_);
      buf_ = 0;
      buflen_ = 0;
   }
}

void DataBlockStrChunk::FillGroupVec(GroupVector &vec,
				     unsigned int &start_index)
{
   if (buflen_ > 0) {
      vec.SetVec(start_index, GetVoidP(), buflen_);
      start_index++;
   }
}

void DataBlockStrChunk::FillGroupVec(const LinearExtent &extent,
				     GroupVector &vec,
				     unsigned int &start_index)
{
   if (&extent == &LinearExtent::full_extent) {
      if (buflen_ > 0) {
	 vec.SetVec(start_index, GetVoidP(), buflen_);
	 start_index++;
      }
   } else {
      LinearExtent myextent(0, buflen_);

      myextent.SubExtent_eq(extent);

      if (myextent.Offset() < buflen_) {
	 vec.SetVec(start_index, static_cast<char *>(buf_) + myextent.Offset(),
		    myextent.Length());
	 start_index++;
      }
   }
}

void DataBlockStrChunk::Resize(size_t newsize)
{
   if (newsize == buflen_) {
      return;
   } else if (newsize == 0) {
      assert(buf_ != 0);

      free(buf_);
      buf_ = 0;
      buflen_ = 0;
   } else {
      if (buflen_ == 0) {
	 assert(buf_ == 0);

	 buf_ = malloc(newsize);
	 if (buf_ != 0) {
	    buflen_ = newsize;
	 }
      } else {
	 assert(buf_ != 0);

	 void *tempbuf = realloc(buf_, newsize);

	 if (tempbuf != 0) {
	    buf_ = tempbuf;
	    buflen_ = newsize;
	 }
      }
   }
}
