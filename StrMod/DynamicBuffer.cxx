/* $Header$ */

// For a log, see ./ChangeLog

#ifdef __GNUG__
#  pragma implementation "DynamicBuffer.h"
#endif

#include "StrMod/DynamicBuffer.h"
#include <cstdlib>
#include <cstring>  // memcpy
#include <new>
#include <cassert>

const STR_ClassIdent DynamicBuffer::identifier(39UL);

DynamicBuffer::DynamicBuffer(unsigned int len) throw(bad_alloc)
{
   resize(len);
}

DynamicBuffer::DynamicBuffer(const void *data,
			     unsigned int len) throw(bad_alloc)
{
   resize(len);
   memcpy(buf_, data, len);
}

DynamicBuffer::~DynamicBuffer()
{
   if (buf_)
   {
      assert(buflen_ > 0);
      free(buf_);
      buf_ = 0;
      buflen_ = 0;
   }
}

void DynamicBuffer::resize(unsigned int newsize) throw(bad_alloc)
{
   #ifndef NDEBUG
   if (buf_ != 0)
   {
      assert((buf_ != 0) && (buflen_ > 0));
   }
   else
   {
      assert((buf_ == 0) && (buflen_ == 0));
   }
   #endif

   if (newsize != buflen_)
   {
      if (newsize == 0)
      {
	 free(buf_);
	 buf_ = 0;
	 buflen_ = newsize;
      }
      else
      {
	 void *newbuf = realloc(buf_, newsize);
	 if ((newbuf == NULL) && (newsize != 0))
	 {
	    throw bad_alloc();
	 }
	 else
	 {
	    buf_ = static_cast<U1Byte *>(newbuf);
	    buflen_ = newsize;
	 }
      }
   }
}
