#ifndef _STR_BufferChunk_H_  // -*-c++-*-

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

#include <StrMod/ChunkVisitor.h>
#include <StrMod/StrChunk.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/LinearExtent.h>
#include <LCore/GenTypes.h>
#include <LCore/Debugable.h>
#include <new>

#define _STR_BufferChunk_H_

/** \class BufferChunk BufferChunk.h StrMod/BufferChunk.h
 * This is an abstract base class for StrChunks that are really just bags of
 * bytes.
 */
class BufferChunk : public StrChunk, virtual public Debugable {
 public:
   class Factory;
   static const STR_ClassIdent identifier;

   /** As a convenience, initialize the protected variables maintained by the
    * derived classes.
    */
   BufferChunk() : buf_(0), buflen_(0)                  { }
   /** Doesn't delete any storage whatsoever.
    *
    * Since this is an abstract class that doesn't ever actually allocate
    * any storage, it's left to the derived classes to do the actual
    * deletion.
    */
   virtual ~BufferChunk()                               { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool invariant() const;

   virtual void printState(ostream &os) const;

   virtual unsigned int Length() const                  { return(buflen_); }

   /** Returns a reference to the byte at index bnum.
    *
    * If bnum is out of range, the behavior is undefined.
    */
   inline U1Byte &operator [](unsigned int bnum);
   /** Gets a void pointer to at least Length() bytes of data.
    *
    * This guarantees that you can read or write to any byte in the valid
    * range using this pointer as long as you make no calls to resize() in
    * between accesses.
    */
   inline void *getVoidP();
   /** Gets a <code>U1Byte</code> pointer to at least <code>Length()</code>
    * bytes of data.
    *
    * This guarantees that you can read or write to any byte in the valid
    * range using this pointer as long as you make no calls to resize() in
    * between accesses.
    */
   inline U1Byte *getCharP();

   /** Change the size of the chunk to newsize.
    *
    * Throws bad_alloc if the allocation fails, just like operator new.
    */
   virtual void resize(unsigned int newsize) throw(bad_alloc) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 protected:
   /** <code>buf_</code> is expected to be maintained by the derived class.
    *
    * This exists so that the above inline functions are truly inline.  It's
    * only allowed to change in the constructor, or in respone to the resize()
    * method being called.
    *
    * <code>buf_</code> is required to be non-null when <code>buflen_</code>
    * is greater than 0.  */
   void *buf_;
   /** <code>buflen_</code> is expected to be maintained by the derived class.
    *
    * This exist so that the above inline functions are truly inline.  It's
    * only allowed to change in the constructor, or in respone to the resize()
    * method being called.
    */
   unsigned int buflen_;

 private:
   static U1Byte junk_;

   // Left undefined on purpose.
   void operator =(const BufferChunk &b);
   BufferChunk(const BufferChunk &b);
};

//-----------------------------inline functions--------------------------------

inline int BufferChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline bool BufferChunk::invariant() const
{
   return((buflen_ == 0) || (buf_ != 0));
}

inline U1Byte &BufferChunk::operator [](unsigned int bnum)
{
   return((bnum < buflen_) ? static_cast<U1Byte *>(buf_)[bnum] : junk_);
}

inline void *BufferChunk::getVoidP()
{
   return((buflen_ > 0) ? buf_ : &junk_);
}

inline U1Byte *BufferChunk::getCharP()
{
   return((buflen_ > 0) ? static_cast<U1Byte *>(buf_) : &junk_);
}

#endif
