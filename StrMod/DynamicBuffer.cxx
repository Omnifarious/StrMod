/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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

// For a log, see ./ChangeLog

#ifdef __GNUG__
#  pragma implementation "DynamicBuffer.h"
#endif

#include "StrMod/DynamicBuffer.h"
#include <cstdlib>
#include <cstring>  // memcpy
#include <new>
#include <cassert>

namespace strmod {
namespace strmod {

const STR_ClassIdent DynamicBuffer::identifier(39UL);

DynamicBuffer::DynamicBuffer(unsigned int len) throw(std::bad_alloc)
{
   resize(len);
}

DynamicBuffer::DynamicBuffer(const void *data,
			     unsigned int len) throw(std::bad_alloc)
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

void DynamicBuffer::resize(unsigned int newsize) throw(std::bad_alloc)
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
	    throw std::bad_alloc();
	 }
	 else
	 {
	    buf_ = static_cast<lcore::U1Byte *>(newbuf);
	    buflen_ = newsize;
	 }
      }
   }
}

};  // End namespace strmod
};  // End namespace strmod
