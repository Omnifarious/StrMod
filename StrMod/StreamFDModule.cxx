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
#ifndef _STR_GroupVector_H_
#   include "StrMod/GroupVector.h"
#endif

#include <unistd.h>  // read  (and maybe sysconf)
#include <sys/types.h>  // writev and struct iovec
#include <sys/uio.h>    // writev and struct iovec
#include <sys/stat.h>
#include <assert.h>
#include <stddef.h>  // NULL
#include <errno.h> // ESUCCESS
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

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0) = 0;

   inline void parentGone()                            { hasparent_ = false; }

 protected:
   inline void triggerRead();
   inline void triggerWrite();
   inline void triggerError();

 private:
   bool_val hasparent_;
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

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0)  { triggerRead(); }
};

//: This is one of the three helper classes for StreamFDModule::FDPollEv
class StreamFDModule::FDPollWrEv : public StreamFDModule::FDPollEv {
 public:
   inline FDPollWrEv(StreamFDModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollWrEv()                                          { }

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0)  { triggerWrite(); }
};

//: This is one of the three helper classes for StreamFDModule::FDPollEv
class StreamFDModule::FDPollErEv : public StreamFDModule::FDPollEv {
 public:
   inline FDPollErEv(StreamFDModule &parent) : FDPollEv(parent)   { }
   virtual ~FDPollErEv()                                          { }

   virtual void TriggerEvent(UNIDispatcher *dispatcher = 0)  { triggerError(); }
};

void StreamFDModule::setErrorIn(ErrCategory ecat, int err)
{
   if (err == ESUCCESS)
   {
      resetErrorIn(ecat);
   }
   errvals[ecat] = err;
}

bool_val StreamFDModule::resetErrorIn(ErrCategory ecat)
{
   if (ecat == ErrFatal)
   {
      return(false);
   }
   if (!hasErrorIn(ecat))
   {
      return(false);
   }

   bool_val retval = false;

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

   if (write_vec_ == NULL)
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
	 write_vec_ = new GroupVector(cur_write_->NumSubGroups());
	 cur_write_->SimpleFillGroupVec(*write_vec_);
	 write_pos_ = 0;
	 write_length_ = write_vec_->TotalLength();
      }
   }

   assert(write_vec_ != NULL);

   // Tell the compiler how I intend to use this value.
   const size_t length = write_length_;

   // This loop is also broken out of when there's an error writing.
   while (write_pos_ < length)
   {
      int written;
      bool_val result;
      GroupVector::IOVecDesc ioveclst;

      result = write_vec_->FillIOVecDesc(write_pos_, ioveclst);
      assert(result == true);
      assert(ioveclst.iovcnt > 0);
      if (ioveclst.iovcnt > MAXIOVCNT)
      {
	 ioveclst.iovcnt = MAXIOVCNT;
      }
      written = ::writev(fd_, ioveclst.iov, ioveclst.iovcnt);
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
	 write_pos_ += written;
      }
   }

   assert(write_pos_ <= length);

   // i.e. == length, with an attempt to recover from a 'broken axle'
   if (write_pos_ >= length)
   {
      if (write_vec_ != NULL)
      {
	 delete write_vec_;
	 write_vec_ = NULL;
      }
      cur_write_.ReleasePtr();
      write_pos_ = 0;
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
			       IOCheckFlags f, bool_val hangdelete)
     : fd_(fd),
       plug_(*this),
       write_pos_(0),
       write_length_(0),
       write_vec_(NULL),
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
}
