/* $Header$ */

// $Log$
// Revision 1.3  1996/02/26 03:43:03  hopper
// Added stuff for new ShallowCopy method.
//
// Revision 1.2  1995/07/23 04:00:32  hopper
// Added #include <string.h> in various places to avoid triggering a bug
// in libg++ 2.7.0
//
// Revision 1.1.1.1  1995/07/22 04:46:47  hopper
// Imported sources
//
 // -> Revision 0.13  1995/04/14  17:07:57  hopper
 // -> Combined revisions 0.12 and 0.12.0.4
 // ->
 // -> Revision 0.12.0.4  1995/04/14  17:06:49  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.12.0.3  1994/06/08  05:12:00  hopper
 // -> Changes necessary to implement a different style of file IO for
 // -> sockets. This feature is only expected to be used by OS/2. Also changed
 // -> the method used to get the last I/O error to do the right thing if we're on
 // -> OS/2.
 // ->
 // -> Revision 0.12.0.2  1994/05/17  06:09:13  hopper
 // -> Changed to use new style of reference counting.
 // ->
 // -> Revision 0.12.0.1  1994/05/16  05:52:15  hopper
 // -> No changes, but I needed a head for the inevitable WinterFire-OS/2
 // -> branch.
 // ->
 // -> Revision 0.12  1994/05/07  03:24:53  hopper
 // -> Changed header files stuff around to be aprroximately right with
 // -> new libraries, and new names & stuff.
 // ->
 // -> Revision 0.11  1992/04/14  04:31:35  hopper
 // -> Addded DataBlockStrChunk::~DataBlockStrChunk(), and also added
 // -> some stuff to allow a maximum size to be specified when
 // -> reading StrChunk's from fd's
 // ->
 // -> Revision 0.10  1992/03/20  03:46:24  hopper
 // -> Genesis! (All the syntax errors are fixed though. :-)
 // ->

#include "StrMod/StrChunk.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static char *StrChunk_CC_rcsID = "$Id$";

static char bigbuf[8192];
extern int errno;

extern "C" int recv(int, void *, int, int);
extern "C" int send(int, const void *, int, int);

const STR_ClassIdent DataBlockStrChunk::identifier(7UL);

DataBlockStrChunk::~DataBlockStrChunk()
{
   if (buf) {
      buf->DelReference();
      if (buf->NumReferences() <= 0)
	 delete buf;
   }
}

void DataBlockStrChunk::SetBuf(StrChunkBuffer *b)
{
   if (buf) {
      buf->DelReference();
      if (buf->NumReferences() == 0)
	 delete buf;
   }
   buf = b;
   if (buf)
      buf->AddReference();
}

void DataBlockStrChunk::SetBuf(const void *b, unsigned int size)
{
   if (!buf || buf->NumReferences() > 1)
      SetBuf(new StrChunkBuffer(b, size));
   else {
      buf->Resize(size);
      memcpy(buf->GetBufAsVoid(), b, size);
   }
}

int DataBlockStrChunk::i_FillFromFd(int fd, int start,
				    int maxsize, int issocket, int flags)
{
   int size;

   if (maxsize <= 0 || maxsize > 8192)
      maxsize = 8192;
   if (!issocket)
      size = read(fd, bigbuf, maxsize);
   else
      size = recv(fd, bigbuf, maxsize, flags);
   if (size < 0) {
#ifdef OS2
      last_errno = sock_errno();
#else
      last_errno = errno;
#endif
      if (start == 0)
	 SetBuf(0);
      else if (buf) {
	 if (buf->NumReferences() == 1)
	    buf->Resize(start);
	 else
	    SetBuf(new StrChunkBuffer(buf->GetBufAsVoid(), start));
      }
   } else {
      last_errno = 0;
      if (!buf) {
	 if (start > 0) {
	    SetBuf(new StrChunkBuffer(start + size));
	    memcpy(buf->GetBufAsChar() + start, bigbuf, size);
	 } else
	    SetBuf(new StrChunkBuffer(bigbuf, size));
      } else {
	 if (buf->NumReferences() != 1) {
	    StrChunkBuffer *newb = new StrChunkBuffer(start + size);

	    if (start > 0)
	       memcpy(newb->GetBufAsVoid(), buf->GetBufAsVoid(), start);
	    SetBuf(newb);
	 } else
	    buf->Resize(start + size);
	 memcpy(buf->GetBufAsChar() + start, bigbuf, size);
      }
   }
   return(size);
}

int DataBlockStrChunk::i_PutIntoFd(int fd, int start,
				   int maxsize, int issocket, int flags)
{
   if (!buf || start < 0 || start >= buf->GetSize())
      return(0);
   else {
      int size;

      if (maxsize <= 0 || maxsize > (buf->GetSize() - start))
	 maxsize = buf->GetSize() - start;
      if (!issocket)
	 size = write(fd, buf->GetBufAsChar() + start, maxsize);
      else
	 size = send(fd, buf->GetBufAsChar() + start, maxsize, flags);
      if (size < 0) {
#ifdef OS2
	 last_errno = sock_errno();
#else
	 last_errno = errno;
#endif
      } else
	 last_errno = 0;
      return(size);
   }
}

DataBlockStrChunk::DataBlockStrChunk(const DataBlockStrChunk &b) : buf(0)
{
   SetBuf(b.buf);
}

StrChunk *DataBlockStrChunk::i_ShallowCopy() const
{
   return(new DataBlockStrChunk(*this));
}
