/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "PreAllocBuffer.h"
#endif

#include "StrMod/PreAllocBuffer.h"
#include <iostream>
#include <cstdlib>  // malloc, realloc, and free.
#include <cstring>  // memcpy

namespace strmod {
namespace strmod {

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
                                  U1Byte * const preallocbuf)
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
	 throw std::bad_alloc();
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

void PreAllocBufferBase::i_printState(std::ostream &os,
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

}  // End namespace strmod
}  // End namespace strmod
