#ifndef _STR_PreAllocBuffer_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog

#include <StrMod/BufferChunk.h>
#include <iosfwd>
#include <new>

#define _STR_PreAllocBuffer_H_

namespace strmod {
namespace strmod {

/** \class PreAllocBufferBase PreAllocBuffer.h StrMod/PreAllocBuffer.h
 * \brief Just a base class that defines the functions for the template class
 * that don't depend on the template argument.
 */
class PreAllocBufferBase : public BufferChunk
{
 private:
   void a_silly_member_function_to_make_sure_a_vtable_is_generated();
   typedef lcore::U1Byte U1Byte;
 public:
   PreAllocBufferBase()                                 { }
   virtual ~PreAllocBufferBase() = 0;

   //: See class Debugable
   bool invariant() const override = 0;

   //: See class Debugable
   void printState(std::ostream &os) const override = 0;

   void resize(unsigned int newsize) override = 0;

 protected:
   void i_destruct(const U1Byte * const preallocbuf);
   void i_resize(const unsigned int newsize,
		 const unsigned int prebufsize,
		 U1Byte * const preallocbuf);
   bool i_invariant(const unsigned int prebufsize,
		    const void * const prebuf) const;
   void i_printState(std::ostream &os,
		     const unsigned int prebufsize,
		     const void * const prebuf) const;
};

//---

/** \class PreAllocBuffer PreAllocBuffer.h StrMod/PreAllocBuffer.h
 * \brief A template class for buffers that contain a certain fixed amount of
 * storage that's not dynamically allocated.
 */
template <unsigned int TInitialAlloc>
class PreAllocBuffer : public PreAllocBufferBase {
 private:
   typedef lcore::U1Byte U1Byte;
 public:
   // There isn't any identifier here because there's no good way (in the
   // identifier system) to generate a unique identifier for every template
   // instantiation.
   inline PreAllocBuffer();
   inline virtual ~PreAllocBuffer();

   inline bool invariant() const override;

   inline void printState(std::ostream &os) const override;

   inline void resize(unsigned int newsize) override;

 private:
   U1Byte preallocbuf_[TInitialAlloc];
};

//-----------------------------inline functions--------------------------------

template <unsigned int TInitialAlloc>
inline PreAllocBuffer<TInitialAlloc>::PreAllocBuffer()
{
}

template <unsigned int TInitialAlloc>
inline PreAllocBuffer<TInitialAlloc>::~PreAllocBuffer()
{
   i_destruct(preallocbuf_);
}

template <unsigned int TInitialAlloc>
inline bool PreAllocBuffer<TInitialAlloc>::invariant() const
{
   return(i_invariant(TInitialAlloc, preallocbuf_));
}

template <unsigned int TInitialAlloc>
inline void PreAllocBuffer<TInitialAlloc>::printState(std::ostream &os) const
{
   return(i_printState(os, TInitialAlloc, preallocbuf_));
}

template <unsigned int TInitialAlloc>
inline void
PreAllocBuffer<TInitialAlloc>::resize(unsigned int newsize)
{
   i_resize(newsize, TInitialAlloc, preallocbuf_);
}

}  // namespace strmod
}  // namespace strmod

#endif
