#ifndef _STR_DynamicBuffer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/BufferChunk.h>

#define _STR_DynamicBuffer_H_

//: A completely dyanmically allocated bag of bytes.
class DynamicBuffer : public BufferChunk {
 public:
   static const STR_ClassIdent identifier;

   DynamicBuffer()                                      { }
   DynamicBuffer(unsigned int len) throw(bad_alloc);
   DynamicBuffer(const void *data, unsigned int len) throw(bad_alloc);
   virtual ~DynamicBuffer();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual void resize(unsigned int newsize) throw(bad_alloc);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void i_DropUnused(const LinearExtent &usedextent,
			     KeepDir keepdir)           { }
};

//-----------------------------inline functions--------------------------------

inline int DynamicBuffer::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || BufferChunk::AreYouA(cid));
}

#endif
