#ifndef _STR_StrChunk_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.4  1996/02/26 03:42:15  hopper
// Added stuff for new ShallowCopy method.
//
// Revision 1.3  1996/02/22 03:52:28  hopper
// Made some changes now that we have ReferenceCounting again.
//
// Revision 1.2  1996/02/12 05:50:49  hopper
// Declared something to be inline that should be inline.
//
// Revision 1.1.1.1  1995/07/22 04:46:50  hopper
// Imported sources
//
// ->Revision 0.28  1995/04/05  04:52:22  hopper
// ->Fixed a stupid mistake.
// ->
// ->Revision 0.27  1995/04/05  03:11:36  hopper
// ->Changed things for integration into the rest of my libraries.
// ->
// ->Revision 0.26  1995/01/13  17:32:57  hopper
// ->Merged versions 0.25, and 0.25.0.8 back together.
// ->
// ->Revision 0.25.0.8  1995/01/13  17:27:54  hopper
// ->Made the #include directives work under OS/2.
// ->
// ->Revision 0.25.0.7  1994/07/18  04:01:03  hopper
// ->Fixed syntax errors that gcc 2.6.0 just found...
// ->Major syntax/semantic change: Uses unsigned char's for everything
// ->now.
// ->
// ->Revision 0.25.0.6  1994/06/20  04:23:25  hopper
// ->Added Resize(unsigned int) method to DataBlockStrChunk.
// ->
// ->Revision 0.25.0.5  1994/06/16  02:19:18  hopper
// ->Added #pragma interface declaration.
// ->
// ->Revision 0.25.0.4  1994/06/08  05:13:09  hopper
// ->Fixed some stupid minor syntax errors/typos.
// ->
// ->Revision 0.25.0.3  1994/06/08  04:55:28  hopper
// ->Added better formatting. Added stuff to support using a different I/O form
// ->for sockets. This will probably only be used by OS2SocketModule.
// ->
// ->Revision 0.25.0.2  1994/05/16  05:21:37  hopper
// ->Removed dependency on my ReferenceCounting class.
// ->
// ->Revision 0.25.0.1  1994/05/16  02:42:46  hopper
// ->No changes, but I needed a head for the inevitable WinterFire-OS/2
// ->branch.
// ->
// ->Revision 0.25  1994/05/07  03:40:55  hopper
// ->Move enclosing #ifndef ... #define bits around. Changed names of some
// ->non-multiple include constants. Changed directories for certain
// ->type of include files.
// ->
// ->Revision 0.24  1994/04/14  15:41:22  hopper
// ->More minor changes.
// ->
// ->Revision 0.23  1994/04/14  15:40:33  hopper
// ->Minor, minor change.
// ->
// ->Revision 0.22  1992/05/28  19:29:03  hopper
// ->Fixed a few stupid syntax errors. They involved not having inline
// ->in the right place.
// ->
// ->Revision 0.21  1992/05/22  03:26:48  hopper
// ->Added some constructors to DataBlockStrChunk that should've
// ->been there already.
// ->
// ->Revision 0.20  1992/04/15  02:14:29  hopper
// ->Fixed a stupid error involving the rcsID string.
// ->
// ->Revision 0.19  1992/04/14  04:36:38  hopper
// ->Fixed some stuff. Added maximum size specs for
// ->FillFromFd, and PutIntoFd
// ->
// ->Revision 0.18  1992/03/20  03:58:15  hopper
// ->Removed some outdated log messages.
// ->
// ->Revision 0.17  1992/03/20  03:54:57  hopper
// ->Added class DataBlockStrChunk, and fixed some silly
// ->things. Most of the code for DataBlockStrChunk is in
// ->StrChunkio.cc
// ->
// ->Revision 0.16  1992/02/10  04:25:10  hopper
// ->Changed StrChunk::ChunkType && StrChunk::IsChunkType to work with
// ->Rev 0.13 of EHaudio++/ChunkType.h
// ->
// -> ........
// ->
// ->Revision 0.11  1991/11/19  05:45:36  hopper
// ->Had to change a delete in ~StrChunkBuffer to a free. See StrChunk.cc
// ->for reason.
// ->(StrChunk.cc 0.10 log file anyway.)
// ->

#ifndef NO_RcsID
static char _EH_StrChunk_H_rcsID[] = "$Id$";
#endif

#include <assert.h>

// ** For reasons why this has to be here, see StrChunk.cc
#include <stdlib.h>

#ifndef _STR_STR_ClassIdent_H_
#  ifndef OS2
#     include <StrMod/STR_ClassIdent.h>
#  else
#     include "str_clas.h"
#  endif
#endif

#include <ibmpp/RefCounting.h>
#include <LCore/Object.h>

#define _STR_StrChunk_H_

//---------------------------class StrChunkBuffer------------------------------

class StrChunkBuffer : public Object, public ReferenceCounting {
 public:
   static const STR_ClassIdent identifier;

   StrChunkBuffer(const void *, unsigned int size);
   StrChunkBuffer(const StrChunkBuffer &b);
   StrChunkBuffer(unsigned int size);
   inline virtual ~StrChunkBuffer();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   unsigned int GetSize() const            { return(length); }
   void Resize(unsigned int size);

   void *GetBufAsVoid() const              { return(data); }
   unsigned char *GetBufAsChar() const     { return((unsigned char *)(data)); }

   inline unsigned char &operator [](unsigned int index) const;

   const StrChunkBuffer &operator =(const StrChunkBuffer &b);

 private:
   unsigned int length;
   void *data;
};

//------------------------------class StrChunk---------------------------------

class StrChunk : public Object {
 public:
   static const STR_ClassIdent identifier;

   StrChunk() : last_errno(0)                    {}
   virtual ~StrChunk()                           {}

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const = 0;

   inline int FillFromFd(int fd, int start = 0, int maxsize = 0);
   inline int PutIntoFd(int fd, int start = 0, int maxsize = 0);

   inline int FillFromSock(int fd, int start = 0,
			   int maxsize = 0, int flags = 0);
   inline int PutIntoSock(int fd, int start = 0,
			  int maxsize = 0, int flags = 0);

   int GetLastErrno() const                      { return(last_errno); }

   virtual const void *GetCVoidP() = 0;

 protected:
   int last_errno;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual int i_FillFromFd(int fd, int start,
			    int maxsize, int issocket, int flags) = 0;
   virtual int i_PutIntoFd(int fd, int start,
			   int maxsize, int issocket, int flags) = 0;
};

//--------------------------class DataBlockStrChunk----------------------------

class DataBlockStrChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   DataBlockStrChunk()                                  { buf = 0; }
   inline DataBlockStrChunk(unsigned int length);
   inline DataBlockStrChunk(unsigned int length, const void *mem);
   virtual ~DataBlockStrChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual unsigned int Length() const;

   virtual unsigned char &operator [](unsigned int bnum) const;

   inline void *GetVoidP() const;
   inline unsigned char *GetCharP() const;
   virtual const void *GetCVoidP()                      { return(GetVoidP()); }

   inline void Resize(unsigned int newsize);

   inline DataBlockStrChunk *ShallowCopy() const;

 protected:
   DataBlockStrChunk(const DataBlockStrChunk &b);

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   void SetBuf(StrChunkBuffer *b);
   void SetBuf(const void *b, unsigned int size);

   virtual int i_FillFromFd(int fd, int start,
			    int maxsize, int issocket, int flags);
   virtual int i_PutIntoFd(int fd, int start,
			   int maxsize, int issocket, int flags);

   virtual StrChunk *i_ShallowCopy() const;

 private:
   StrChunkBuffer *buf;

   void operator =(const DataBlockStrChunk &b);
};

//------------------------inline functions for StrChunk------------------------

inline int StrChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

inline int StrChunk::FillFromFd(int fd, int start, int maxsize)
{
   return(i_FillFromFd(fd, start, maxsize, 0, 0));
}

inline int StrChunk::PutIntoFd(int fd, int start, int maxsize)
{
   return(i_PutIntoFd(fd, start, maxsize, 0, 0));
}

inline int StrChunk::FillFromSock(int fd, int start, int maxsize, int flags)
{
   return(i_FillFromFd(fd, start, maxsize, 1, flags));
}

inline int StrChunk::PutIntoSock(int fd, int start, int maxsize, int flags)
{
   return(i_PutIntoFd(fd, start, maxsize, 1, flags));
}

//--------------------inline functions for DataBlockStrChunk-------------------

inline int DataBlockStrChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline unsigned int DataBlockStrChunk::Length() const
{
   return(buf ? buf->GetSize() : 0);
}

inline unsigned char &DataBlockStrChunk::operator [](unsigned int bnum) const
{
   assert(buf != 0); // See below in implementation of operator [] in
                     // StrChunkBuffer for details of why I'm doing this.
   return((*buf)[bnum]);
}

inline void *DataBlockStrChunk::GetVoidP() const
{
   return((buf == 0) ? 0 : buf->GetBufAsVoid());
}

inline unsigned char *DataBlockStrChunk::GetCharP() const
{
   return((buf == 0) ? 0 : buf->GetBufAsChar());
}

inline void DataBlockStrChunk::Resize(unsigned int newsize)
{
   if (buf)
      buf->Resize(newsize);
   else
      SetBuf(new StrChunkBuffer(newsize));
}

inline DataBlockStrChunk *DataBlockStrChunk::ShallowCopy() const
{
   return(static_cast<DataBlockStrChunk *>(i_ShallowCopy()));
}

inline DataBlockStrChunk::DataBlockStrChunk(unsigned int length) : buf(0)
{
   SetBuf(new StrChunkBuffer(length));
}

inline DataBlockStrChunk::DataBlockStrChunk(unsigned int length,
					    const void *mem) :
     buf(0)
{
   SetBuf(mem, length);
}

//---------------------inline functions for StrChunkBuffer--------------------

inline StrChunkBuffer::~StrChunkBuffer()
{
   if (data)
      free(data);  // *yech* :-P
}

inline int StrChunkBuffer::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) ||
	  Object::AreYouA(cid) ||
	  ReferenceCounting::AreYouA(cid));
}

inline unsigned char &StrChunkBuffer::operator [](unsigned int index) const
{
   assert(index < length); // This is a FATAL error. Programs that do this
               // shouldn't keep running. (Also, I want to be able turn off as
               // much extraneous debugging as possible speed, and assertions
               // can be turned of using a flag.

   return((GetBufAsChar())[index]);
}

#endif
