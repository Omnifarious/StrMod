#ifndef _ulaw_StrChunk_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/22 04:46:50  hopper
 // Initial revision
 //

#ifndef NO_RcsID
static char _ulaw_StrChunk_H_rcsID[] =
      "$Id$";
#endif

#define _ulaw_StrChunk_H_

class Ulaw8_StrChunk : public StrChunk {
   StrChunkBuffer *buf;

 protected:
   void SetBuf(StrChunkBuffer *b);

 public:
   virtual unsigned int Length() const

#endif
