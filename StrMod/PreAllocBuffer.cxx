/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "PreAllocBuffer.h"
#endif

#include "StrMod/PreAllocBuffer.h"
#include <iostream>
#include <cstdlib>  // malloc, realloc, and free.
#include <cstring>  // memcpy

const STR_ClassIdent PreAllocBufferBase::identifier(38UL);

void
PreAllocBufferBase::a_silly_member_function_to_make_sure_a_vtable_is_generated()
{
}

PreAllocBufferBase::~PreAllocBufferBase()
{
}

void PreAllocBufferBase::i_destruct(const U1Byte * const preallocbuf)
{
   if (buf_ != preallocbuf)
   {
      if (buf_ != 0)
      {
	 free(buf_);
      }
   }
   buf_ = 0;
   buflen_ = 0;
}

void PreAllocBufferBase::i_resize(const unsigned int newsize,
											 const unsigned int prebufsize,
											 U1Byte * const preallocbuf) throw(bad_alloc)
{
   if (newsize <= prebufsize)
   {
      if (buf_ != preallocbuf)
      {
	 if (buf_ != 0)
	 {
	    memcpy(preallocbuf, buf_, newsize);
	    free(buf_);
	    buf_ = 0;
	 }
	 if (newsize != 0)
	 {
	    buf_ = preallocbuf;
	 }
      }
      buflen_ = newsize;
   }
   else
   {
      U1Byte *newbuf = 0;

      if ((buf_ == preallocbuf) || (buf_ == 0))
      {
	 newbuf = static_cast<U1Byte *>(malloc(newsize));
	 if ((newbuf != 0) && (buf_ != 0))
	 {
	    assert(buflen_ <= newsize);
	    memcpy(newbuf, buf_, buflen_);
	 }
      }
      else
      {
	 newbuf = static_cast<U1Byte *>(realloc(buf_, newsize));
      }
      if (newbuf == 0)
      {
	 throw bad_alloc();
      }
      buf_ = newbuf;
      buflen_ = newsize;
   }
}

bool PreAllocBufferBase::i_invariant(const unsigned int prebufsize,
				     const void * const prebuf) const
{
   if (buflen_ == 0)
   {
      return((buf_ == 0) || (buf_ == prebuf));
   }
   else if (buflen_ <= prebufsize)
   {
      return(buf_ == prebuf);
   }
   else
   {
      return(buf_ != 0);
   }
}

void PreAllocBufferBase::i_printState(ostream &os,
				      const unsigned int prebufsize,
				      const void * const prebuf) const
{
   os << "PreAllocBuffer<" << prebufsize << ">(buf_ == ";
   if (buf_ == prebuf)
   {
      os << "preallocbuf_";
   }
   else
   {
      os << buf_;
   }
   os << ", buflen_ == " << buflen_ << ", prebufsize == " << prebufsize << ")";
}
