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

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "StreamFDModule.h"
#endif

#include "StrMod/StreamFDModule.h"
#ifndef _STR_DynamicBuffer_H_
#   include "StrMod/DynamicBuffer.h"
#endif
#ifndef _STR_EOFStrChunk_H_
#   include "StrMod/EOFStrChunk.h"
#endif
#ifndef _STR_UseTrackingVisitor_H_
#   include "StrMod/UseTrackingVisitor.h"
#endif

#include <unistd.h>  // read  (and maybe sysconf)
#include <sys/types.h>  // writev and struct iovec
#include <sys/uio.h>    // writev and struct iovec
#include <sys/stat.h>
#include <assert.h>
#include <stddef.h>  // NULL
#include <errno.h> // ESUCCESS
#include <vector>
#ifndef ESUCCESS
#  define ESUCCESS 0
#endif

typedef struct stat statbuf_t;

const STR_ClassIdent StreamFDModule::identifier(8UL);
const STR_ClassIdent StreamFDModule::FPlug::identifier(9UL);

//: A parent class for the three sub event classes.
// <p>The sub event classes don't do anything except call parent class
// protected functions.  The only reason they exist is to avoid having
// a switch statement in the parent.</p>
class StreamFDModule::FDPollEv : public UNIXpollManager::PollEvent {
 public:
   inline FDPollEv(StreamFDModule &parent);
   virtual ~FDPollEv()                                 { }

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0) = 0;

   inline void parentGone()                            { hasparent_ = false; }

 protected:
   inline void triggerRead();
   inline void triggerWrite();
   inline void triggerError();

 private:
   bool hasparent_;
   StreamFDModule &parent_;
};

inline StreamFDModule::FDPollEv::FDPollEv(StreamFDModule &parent)
     : hasparent_(true), parent_(parent)
{
}

inline void StreamFDModule::FDPollEv::triggerRead()
{
   // cerr << "In triggerRead\n";
   if (hasparent_)
   {
      unsigned int condbits = getCondBits();
      setCondBits(0);
      parent_.eventRead(condbits);
   }
}

inline void StreamFDModule::FDPollEv::triggerWrite()
{
   // cerr << "In triggerWrite\n";
   if (hasparent_)
   {
      unsigned int condbits = getCondBits();
      setCondBits(0);
      parent_.eventWrite(condbits);
   }
}

inline void StreamFDModule::FDPollEv::triggerError()
{
   // cerr << "In triggerError\n";
   if (hasparent_)
   {
      unsigned int condbits = getCondBits();
      setCondBits(0);
      parent_.eventError(condbits);
   }
}

//: This is one of the three helper classes for StreamFDModule::FDPollEv
class StreamFDModule::FDPollRdEv : public StreamFDModule::FDPollEv {
 public:
   inline FDPollRdEv(StreamFDModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollRdEv()                                          { }

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0)  { triggerRead(); }
};

//: This is one of the three helper classes for StreamFDModule::FDPollEv
class StreamFDModule::FDPollWrEv : public StreamFDModule::FDPollEv {
 public:
   inline FDPollWrEv(StreamFDModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollWrEv()                                          { }

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0)  { triggerWrite(); }
};

//: This is one of the three helper classes for StreamFDModule::FDPollEv
class StreamFDModule::FDPollErEv : public StreamFDModule::FDPollEv {
 public:
   inline FDPollErEv(StreamFDModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollErEv()                                          { }

   virtual void triggerEvent(UNIDispatcher *dispatcher = 0)  { triggerError(); }
};

/**
 * The ChunkVisitor that gathers data for the writev calls.
 */
class StreamFDModule::BufferList : public UseTrackingVisitor {
 public:
   //! ChunkVisitors never have very interesting constructors
   // Do ignore zeros though.  When iterating over data, zero length chunks
   // and data extents are pointless.
   inline BufferList() : UseTrackingVisitor(true)  { }
   //! And rarely interesting destructors either.
   virtual ~BufferList()                    { }

   /**
    * Visit the StrChunk DAG rooted at chunk, filling up iovecs_ with what I
    * find.
    *
    * @param chunk The StrChunk to visit.
    */
   void startChunk(const StrChunkPtr &chunk)
   {
      iovecs_.clear();
      totalbytes_ = curbyte_ = 0;
      curvec_ = 0;
      curchunk_ = chunk;
      try
      {
         startVisit(chunk);
      }
      catch (...)
      {
         curchunk_.ReleasePtr();
         throw;
      }
      if (iovecs_.size() <= 0)
      {
         curchunk_.ReleasePtr();
      }
      else
      {
         curvec_ = iovecs_.begin();
      }
   }

   //! How many bytes are left in the currently visited StrChunk?
   size_t bytesLeft() const                { return totalbytes_ - curbyte_; }
   //! What iovec should be passed to the writev call?
   inline iovec *getIOVec() const          { return curvec_; }
   //! How many iovec structures are left?
   inline size_t numVecs() const           { return iovecs_.end() - curvec_; }
   //! Move forward by numbytes, setting it up so the other functions return the right values.
   void advanceBy(size_t numbytes);

 protected:
   /*!
    * I don't care about chunks, just data (because iovecs are all about data)
    * so do nothing when told about a chunk.
    */
   virtual void use_visitStrChunk(const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(halt_visitation)               { }

   /*!
    * Add this new chunk of data to our list.
    */
   virtual void use_visitDataBlock(const void *start, size_t len,
                                   const void *realstart, size_t reallen)
      throw(halt_visitation)
   {
      // Many routines depend on this if statement to ensure that there are no
      // zero length extents.
      if (len <= 0)
      {
         return;
      }

      iovec data = {const_cast<void *>(start), len};

      iovecs_.push_back(data);
      totalbytes_ += len;
   }

 private:
   typedef vector<iovec> iovecvec;
   StrChunkPtr curchunk_;
   size_t totalbytes_;
   size_t curbyte_;
   iovecvec::iterator curvec_;
   iovecvec iovecs_;

   // Private and left undefined on purpose.
   BufferList(const BufferList &b);
   void operator =(const BufferList &b);
};

inline BufferList::BufferList()
     : UseTrackingVisitor(true), totalbytes_(0), curbyte_(0), curvec_(0)
{
}

inline void StreamFDModule::BufferList::advanceBy(size_t numbytes)
{
   if (totalbytes_ <= 0)
   {
      assert(curbyte_ == 0);
      return;
   }
   if ((curbyte_ + numbytes) >= totalbytes_)
   {
      totalbytes_ = curbyte_ = 0;
      curchunk_.ReleasePtr();
      curvec_ = 0;
      iovecs_.clear();
      return;
   }

   for (size_t bytestomove = numbytes; bytestomove > 0; )
   {
      if (curvec_->iov_len <= bytestomove)
      {
         bytestomove -= curvec_->iov_len;
         ++curvec_;
      }
      else
      {
         curvec_->iov_base =
            static_cast<unsigned char *>(curvec_->iov_base) + bytestomove;
         curvec_->iov_len -= bytestomove;
         bytestomove = 0;
      }
   }
   curbyte_ += numbytes;
   assert(curvec_ != iovecs_.end());
}

void StreamFDModule::setErrorIn(ErrCategory ecat, int err)
{
   if (err == ESUCCESS)
   {
      resetErrorIn(ecat);
   }
   errvals[ecat] = err;
}

bool StreamFDModule::resetErrorIn(ErrCategory ecat)
{
   if (ecat == ErrFatal)
   {
      return(false);
   }
   if (!hasErrorIn(ecat))
   {
      return(false);
   }

   bool retval = false;

   switch (ecat)
   {
    case ErrRead:
      errvals[ErrRead] = ESUCCESS;
      maybeShouldReadFD();
      retval = true;
      break;
    case ErrWrite:
      errvals[ErrWrite] = ESUCCESS;
      maybeShouldWriteFD();
      retval = true;
      break;
    case ErrOther:
      errvals[ErrOther] = ESUCCESS;
      maybeShouldReadFD();
      maybeShouldWriteFD();
      retval = true;
      break;
    default:
      break;
   }
   return(retval);
}

void StreamFDModule::setReadEOF(bool newval)
{
   flags_.readeof = newval;
   maybeShouldReadFD();
}

void StreamFDModule::resetReadEOF()
{
   if (flags_.readeof)
   {
      flags_.readeof = 0;
      maybeShouldReadFD();
   }
}

size_t StreamFDModule::getBestChunkSize() const
{
   static const size_t default_size = 4096U;
   static const size_t smallest = 64U;
   static const size_t largest = (128U * 1024U);

   size_t retval = default_size;
   statbuf_t sbuf;

   if ((fstat(fd_, &sbuf) == 0) && (sbuf.st_blksize > 0))
   {
      size_t blksize = sbuf.st_blksize;  // Known positive

      if (blksize < smallest)
      {
	 retval = smallest;
      }
      else if (blksize > largest)
      {
	 retval = largest;
      }
      else
      {
	 retval = blksize;
      }
   }
   return(retval);
}

void StreamFDModule::plugWrite(const StrChunkPtr &ptr)
{
   assert(!cur_write_);
   cur_write_ = ptr;
   if (cur_write_->AreYouA(EOFStrChunk::identifier))
   {
      // Try to get around some things that end up blocking when they close
      // the 'write' half.
      setWriteableFlagFor(&plug_, false);
      maybeShouldWriteFD();
   }
   else
   {
      doWriteFD();
   }
}

const StrChunkPtr StreamFDModule::plugRead()
{
   assert(buffed_read_);

   StrChunkPtr retval = buffed_read_;
   buffed_read_.ReleasePtr();
   if (!hasErrorIn(ErrRead) && !hasErrorIn(ErrFatal) && !readEOF() && fd_ >= 0)
   {
      doReadFD();
   }
   else
   {
      setReadableFlagFor(&plug_, false);
      maybeShouldReadFD();
   }
   return(retval);
}

void StreamFDModule::doReadFD()
{
   // cerr << fd_ << ": In doReadFD\n";
   assert(!buffed_read_);
   assert(!hasErrorIn(ErrRead));
   assert(!hasErrorIn(ErrFatal));
   assert(!readEOF());
   assert(fd_ >= 0);

   ssize_t size = -1;
   int myerrno = ESUCCESS;

   {
      // A normal pointer offers a speed advantage, and we don't know whether
      // we want to set buffed_read until the read succeeds.
      size_t maxsize = getMaxChunkSize();
      DynamicBuffer *dbchunk = new DynamicBuffer(maxsize);

      errno = 0;
      // cerr << "Reading...\n";
      size = ::read(fd_, dbchunk->getVoidP(), maxsize);
      // I may have an error, capture errno to make sure nothing happens to it.
      myerrno = errno;
      // cerr << fd_ << ": just read " << size << " bytes.\n";

      if (size > 0) {
	 dbchunk->resize(size);
	 buffed_read_ = dbchunk;
//  	 cerr << fd_ << ": just read: <";
//  	 cerr.write(dbchunk->GetCharP(), dbchunk->Length());
//  	 cerr << ">\n";
	 dbchunk = 0;
      } else {
	 delete dbchunk;
	 dbchunk = 0;
      }

      assert(dbchunk == 0);
   }

   if (size <= 0)
   {
      // cerr << "Handling read error.\n";
      assert(!buffed_read_);

      if (size == 0)
      {
	 setReadEOF(true);  // We've read the EOF marker.
	 // cerr << fd_ << ": read EOF\n";
	 if (flags_.chunkeof)
	 {
	    buffed_read_ = new EOFStrChunk;
	    // cerr << fd_ << ": sending EOF chunk\n";
	 }
      }
      else  // size MUST be < 0, and so errno must also be set.
      {
	 // myerrno was set up there right after the read call.
	 if (myerrno != EAGAIN)
	 {
	    // EAGAIN just means I need to read later, so it's OK, but anything
	    // else isn't.
	    // cerr << "Handling non-EAGAIN error.\n";
	    // cerr << fd_ << ": setting ErrRead to " << myerrno << "\n";
	    setErrorIn(ErrRead, myerrno);
	 }
      }
   }
   else  // size > 0
   {
      assert(buffed_read_);
   }
   if (!buffed_read_)
   {
      // cerr << "We're not readable now.\n";
      setReadableFlagFor(&plug_, false);
      maybeShouldReadFD();
   }
   else
   {
      // cerr << "We're readable now!\n";
      // cerr << "Currently plug ";
//        if (plug_.isReadable())
//        {
//  	 // cerr << "is readable ";
//        }
//        if (plug_.isWriteable())
//        {
//  	 // cerr << "is writeable ";
//        }
//        cerr << "\n";
      setReadableFlagFor(&plug_, true);
   }
}

void StreamFDModule::doWriteFD()
{
   // cerr << fd_ << ": in doWriteFD()\n";
#ifndef MAXIOVCNT  // UnixWare 7 has this undefined.  *sigh*
#ifndef _SC_IOV_MAX  // Linux has this undefined.  *bigger sigh*
   static const int MAXIOVCNT = 16;
#else
   static const int MAXIOVCNT = sysconf(_SC_IOV_MAX);
#endif
#endif

   assert(cur_write_);
   assert(!hasErrorIn(ErrWrite));
   assert(!hasErrorIn(ErrFatal));
   assert(fd_ >= 0);

   if (curbuflist_.bytesLeft() <= 0)
   {
      if (cur_write_->AreYouA(EOFStrChunk::identifier))
      {
	 cur_write_.ReleasePtr();
	 if (writeEOF())
	 {
	    setWriteableFlagFor(&plug_, true);
	 }
	 else
	 {
	    setWriteableFlagFor(&plug_, false);
	 }
	 return;
      }
      else
      {
         curbuflist_.startChunk(cur_write_);
      }
   }

   assert(curbuflist_.bytesLeft() > 0);

   // Tell the compiler how I intend to use this value.
   size_t length = curbuflist_.bytesLeft();

   // This loop is also broken out of when there's an error writing.
   while (length > 0)
   {
      int written;
      bool result;
      size_t numvecs = curbuflist_.numVecs();
      if (numvecs > MAXIOVCNT)
      {
	 numvecs = MAXIOVCNT;
      }
      written = ::writev(fd_, curbuflist_.getIOVec(), numvecs);
      // cerr << fd_ << ": just wrote: <";
      // cerr.write(ioveclst.iov[0].iov_base, ioveclst.iov[0].iov_len);
      // cerr << ">\n";
      // Save errno for later to make sure it isn't clobbered.
      int myerrno = errno;

      if (written < 0)
      {
	 if (myerrno != EAGAIN)
	 {
	    setErrorIn(ErrWrite, myerrno);
	 }
	 break;
      }
      else
      {
         curbuflist_.advanceBy(written);
         length = curbuflist_.bytesLeft();
      }
   }

   if (curbuflist_.bytesLeft() <= 0)
   {
      cur_write_.ReleasePtr();
   }

   if (!cur_write_ && !(hasErrorIn(ErrWrite) || hasErrorIn(ErrOther)))
   {
      setWriteableFlagFor(&plug_, true);
   }
   else
   {
      setWriteableFlagFor(&plug_, false);
      maybeShouldWriteFD();
   }
}

bool StreamFDModule::writeEOF()
{
   if (fd_ >= 0)
   {
      ::close(fd_);
      pollmgr_.freeFD(fd_);
      fd_ = -1;
   }
//   cerr << fd_ << ": setting ErrRead to " << EBADF << "\n";
   setErrorIn(ErrRead, EBADF);
   setErrorIn(ErrWrite, EBADF);
   setErrorIn(ErrFatal, EBADF);
   return(false);
}

void StreamFDModule::maybeShouldReadFD()
{
//     cerr << "In maybeShouldReadFD\n";
   if (!flags_.checkingrd && (fd_ >= 0) && !readEOF() && !hasErrorIn(ErrRead)
       && !hasErrorIn(ErrOther) && !hasErrorIn(ErrFatal) && !buffed_read_)
   {
      static const unsigned int condbits = UNIXpollManager::FD_Readable;

      pollmgr_.registerFDCond(fd_, condbits, readev_);
      flags_.checkingrd = true;
   }
//     else
//     {
//        cerr << "fd: " << fd_ << " ";
//        if (flags_.checkingrd)
//        {
//  	 cerr << "flags_.checkingrd ";
//        }
//        if (fd_ < 0)
//        {
//  	 cerr << "fd_ < 0 ";
//        }
//        if (readEOF())
//        {
//  	 cerr << "readEOF() ";
//        }
//        if (hasErrorIn(ErrRead))
//        {
//  	 cerr << "hasErrorIn(ErrRead) ";
//        }
//        if (hasErrorIn(ErrOther))
//        {
//  	 cerr << "hasErrorIn(ErrOther) ";
//        }
//        if (hasErrorIn(ErrFatal))
//        {
//  	 cerr << "hasErrorIn(ErrFatal) ";
//        }
//        if (buffed_read_)
//        {
//  	 cerr << "buffed_read_ ";
//        }
//        cerr << "\n";
//     }
}

void StreamFDModule::maybeShouldWriteFD()
{
//     cerr << "In maybeShouldWriteFD\n";
   if (!flags_.checkingwr && (fd_ >= 0) && !hasErrorIn(ErrWrite)
       && !hasErrorIn(ErrOther) && !hasErrorIn(ErrFatal) && cur_write_)
   {
      static const unsigned int condbits = UNIXpollManager::FD_Writeable;

//        cerr << "fd: " << fd_ << " registering for writing.\n";
      pollmgr_.registerFDCond(fd_, condbits, writeev_);
      flags_.checkingwr = true;
   }
}

void StreamFDModule::eventRead(unsigned int condbits)
{
   // cerr << fd_ << ": In eventRead(" << condbits << ")\n";
   flags_.checkingrd = false;
   if (!(condbits & UNIXpollManager::FD_Readable))
   {
      setErrorIn(ErrFatal, -1);
   }
   else
   {
//      if (!hasErrorIn(ErrRead) && !hasErrorIn(ErrFatal)
//	  && !readEOF() && (fd_ >= 0) && !buffed_read_)
      if (!buffed_read_)
      {
	 doReadFD();
      }
//        cerr << "Calling maybeShouldReadFD\n";
      maybeShouldReadFD();
   }
}

void StreamFDModule::eventWrite(unsigned int condbits)
{
//     cerr << fd_ << ": In eventWrite(" << condbits << ")\n";
   flags_.checkingwr = false;
   if (!(condbits & UNIXpollManager::FD_Writeable))
   {
      setErrorIn(ErrFatal, -1);
   }
   else
   {
      if (cur_write_)
      {
//  	 cerr << fd_ << ": in eventWrite cur_write_\n";
	 doWriteFD();
//  	 cerr << fd_ << ": after doWriteFD\n";
      }
      else if (!hasErrorIn(ErrFatal) && !hasErrorIn(ErrWrite) && (fd_ >= 0))
      {
//  	 cerr << fd_ << ": in eventWrite !cur_write_\n";
	 setWriteableFlagFor(&plug_, true);
      }
//        cerr << fd_ << ": after\n";
      maybeShouldWriteFD();
   }
}

void StreamFDModule::eventError(unsigned int condbits)
{
//      cerr << "In eventError\n";
   if (condbits & (UNIXpollManager::FD_Writeable
		   | UNIXpollManager::FD_Readable
		   | UNIXpollManager::FD_Invalid))
   {
      setErrorIn(ErrFatal, -1);
   }
   else
   {
      setErrorIn(ErrOther, -1);
   }
}

StreamFDModule::StreamFDModule(int fd, UNIXpollManager &pollmgr,
			       IOCheckFlags f, bool hangdelete)
     : fd_(fd),
       plug_(*this),
       curbuflist_(*(new BufferList)),
       max_block_size_(4096),
       readevptr_(NULL),
       writeevptr_(NULL),
       errorevptr_(NULL),
       pollmgr_(pollmgr)
{
   errvals[ErrRead] = errvals[ErrWrite]
      = errvals[ErrOther] = errvals[ErrFatal] = ESUCCESS;
   setMaxToBest();
   flags_.hangdelete = hangdelete;
   flags_.plugmade = flags_.hangdelete =
      flags_.readeof = flags_.chunkeof = false;
   // This seems a bit odd, but if you claim to be checking read and
   // write when you really aren't, then the class will never get
   // around to doing it.
   flags_.checkingrd = flags_.checkingwr = true;
   readev_ = readevptr_ = new FDPollRdEv(*this);
   writeev_ = writeevptr_ = new FDPollWrEv(*this);
   errorev_ = errorevptr_ = new FDPollErEv(*this);
   pollmgr_.registerFDCond(fd_,
			   UNIXpollManager::FD_Error
			   | UNIXpollManager::FD_Closed
			   | UNIXpollManager::FD_Invalid,
			   errorev_);
   if ((f == CheckBoth) || (f == CheckRead))
   {
//        cerr << "Registering for reads.\n";
      pollmgr_.registerFDCond(fd_, UNIXpollManager::FD_Readable, readev_);
   }
   if ((f == CheckBoth) || (f == CheckWrite))
   {
//        cerr << "Registering for writes.\n";
      pollmgr_.registerFDCond(fd_, UNIXpollManager::FD_Writeable, writeev_);
   }
}

StreamFDModule::~StreamFDModule()
{
   readevptr_->parentGone();
   writeevptr_->parentGone();
   errorevptr_->parentGone();
   if (fd_ >= 0)
   {
      ::close(fd_);
      pollmgr_.freeFD(fd_);
      fd_ = -1;
   }
   delete &curbuflist_;
}
