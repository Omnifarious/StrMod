#ifndef _STR_DynamicBuffer_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/BufferChunk.h>

#define _STR_DynamicBuffer_H_

namespace strmod {
namespace strmod {

/** \class DynamicBuffer DynamicBuffer.h StrMod/DynamicBuffer.h
 * \brief A completely dyamically allocated bag of bytes.
 */
class DynamicBuffer : public BufferChunk {
 public:
   static const STR_ClassIdent identifier;

   DynamicBuffer()                                      { }
   DynamicBuffer(unsigned int len) throw(std::bad_alloc);
   DynamicBuffer(const void *data, unsigned int len) throw(std::bad_alloc);
   virtual ~DynamicBuffer();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual void resize(unsigned int newsize) throw(std::bad_alloc);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline int DynamicBuffer::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || BufferChunk::AreYouA(cid));
}

};  // namespace strmod
};  // namespace strmod

#endif
