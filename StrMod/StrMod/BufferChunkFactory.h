#ifndef _STR_BufferChunkFactory_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1999/09/16 01:41:21  hopper
// An interface for things that create BufferChunks.
//

#include <StrMod/BufferChunk.h>
#include <StrMod/STR_ClassIdent.h>
#include <LCore/Protocol.h>

#define _STR_BufferChunkFactory_H_

class BufferChunk::Factory : virtual public Protocol {
 public:
   static const STR_ClassIdent identifier;

   Factory()                                            { }
   virtual ~Factory()                                   { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual BufferChunk *makeChunk() = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline int BufferChunk::Factory::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
