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

/* $URL$ */
// $Rev$

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

#include <UniEvent/UNIXError.h>
#include <UniEvent/EventPtr.h>
#include <UniEvent/Event.h>
#include <UniEvent/Dispatcher.h>

#include <LCore/enum_set.h>

#include <vector>
#include <new>
#include <unistd.h>  // read  (and maybe sysconf)
#include <sys/types.h>  // writev and struct iovec
#include <sys/uio.h>    // writev and struct iovec
#include <sys/stat.h>
#include <cassert>
#include <cstddef>  // NULL
#include <cerrno> // ESUCCESS
#ifndef ESUCCESS
#  define ESUCCESS 0
#endif

namespace strmod {
namespace strmod {

typedef struct stat statbuf_t;
using unievent::Dispatcher;
using unievent::UnixEventRegistry;
using unievent::UNIXError;
using lcore::LCoreError;

/** \class StreamFDModule::EvMixin
 * \brief A parent class for StreamFDModule events that provides some behavior
 * for the module being deleted.
 */
class StreamFDModule::EvMixin {
 public:
   /** It's a constructor.
    * @param parent The StreamFDModule that this is an event for.
    */
   inline EvMixin(StreamFDModule &parent)
        : hasparent_(true), parent_(parent)
   {
   }
   //! Virtual because this is a mixin designed to have derived classes.
   virtual ~EvMixin()                           { assert(hasparent_ == false); }

   //! This method is called by StreamFDModule when it goes away.
   inline void parentGone()                     { hasparent_ = false; }

 protected:
   //! Call the right StreamFDModule function for the event.
   inline void triggerRead();
   //! Call the right StreamFDModule function for the event.
   inline void triggerWrite();
   //! Call the right StreamFDModule function for the event.
   inline void triggerError();
   //! Call the right StreamFDModule function for the event.
   inline void triggerResumeRead();
   //! Call the right StreamFDModule function for the event.
   inline void triggerResumeWrite();

 private:
   bool hasparent_;
   StreamFDModule &parent_;
};

inline void StreamFDModule::EvMixin::triggerRead()
{
   // cerr << "In triggerRead\n";
   if (hasparent_)
   {
      parent_.eventRead();
   }
}

inline void StreamFDModule::EvMixin::triggerWrite()
{
   // cerr << "In triggerWrite\n";
   if (hasparent_)
   {
      parent_.eventWrite();
   }
}

inline void StreamFDModule::EvMixin::triggerError()
{
   // cerr << "In triggerError\n";
   if (hasparent_)
   {
      parent_.eventError();
   }
}

inline void StreamFDModule::EvMixin::triggerResumeRead()
{
   if (hasparent_)
   {
      parent_.eventResumeRead();
   }
}

inline void StreamFDModule::EvMixin::triggerResumeWrite()
{
   if (hasparent_)
   {
      parent_.eventResumeWrite();
   }
}

//! This is one of the five helper classes for StreamFDModule::EvMixin
class StreamFDModule::FDPollRdEv
   : public StreamFDModule::EvMixin, public unievent::Event
{
 public:
   inline FDPollRdEv(StreamFDModule &parent) : EvMixin(parent)  { }
   virtual ~FDPollRdEv() = default;

   void triggerEvent(Dispatcher * = 0) override {
      triggerRead();
   }
};

//! This is one of the five helper classes for StreamFDModule::EvMixin
class StreamFDModule::FDPollWrEv
   : public StreamFDModule::EvMixin, public unievent::Event
{
 public:
   inline FDPollWrEv(StreamFDModule &parent) : EvMixin(parent)  { }
   virtual ~FDPollWrEv() = default;

   void triggerEvent(Dispatcher * = 0) override {
      triggerWrite();
   }
};

//! This is one of the five helper classes for StreamFDModule::EvMixin
class StreamFDModule::FDPollErEv
   : public StreamFDModule::EvMixin, public unievent::Event
{
 public:
   inline FDPollErEv(StreamFDModule &parent) : EvMixin(parent)  { }
   virtual ~FDPollErEv() = default;

   void triggerEvent(Dispatcher * = 0) override{
      triggerError();
   }
};

//! This is one of the five helper classes for StreamFDModule::EvMixin
class StreamFDModule::ResumeReadEv
   : public StreamFDModule::EvMixin, public unievent::Event
{
 public:
   ResumeReadEv(StreamFDModule &parent)
        : EvMixin(parent)
   {
   }
   virtual ~ResumeReadEv() = default;

   void triggerEvent(Dispatcher * = 0) override
   {
      triggerResumeRead();
   }
};

//! This is one of the five helper classes for StreamFDModule::EvMixin
class StreamFDModule::ResumeWriteEv
   : public StreamFDModule::EvMixin, public unievent::Event
{
 public:
   ResumeWriteEv(StreamFDModule &parent)
        : EvMixin(parent)
   {
   }
   virtual ~ResumeWriteEv() = default;

   void triggerEvent(Dispatcher * = 0) override
   {
      triggerResumeWrite();
   }
};

/**
 * The ChunkVisitor that gathers data for the writev calls.
 */
class StreamFDModule::BufferList : public UseTrackingVisitor {
 public:
   //! ChunkVisitors never have very interesting constructors
   // Do ignore zeros though.  When iterating over data, zero length chunks
   // and data extents are pointless.
   inline BufferList();
   //! And rarely interesting destructors either.
   virtual ~BufferList() = default;

   /**
    * Visit the StrChunk DAG rooted at chunk, filling up iovecs_ with what I
    * find.
    *
    * @param chunk The StrChunk to visit.
    */
   void startChunk(const StrChunkPtr &chunk)
   {
      iovecs_.clear();
      curvecidx_ = 0;
      totalbytes_ = curbyte_ = 0;
      curchunk_ = chunk;
      try
      {
         startVisit(chunk);
      }
      catch (...)
      {
         curchunk_.reset();
         throw;
      }
      if (iovecs_.size() <= 0)
      {
         curchunk_.reset();
      }
      else
      {
         curvecidx_ = 0;
      }
   }

   //! How many bytes are left in the currently visited StrChunk?
   size_t bytesLeft() const              { return totalbytes_ - curbyte_; }
   //! What iovec should be passed to the writev call?
   inline const iovec *getIOVec() const  { return &iovecs_[curvecidx_]; }
   //! How many iovec structures are left?
   inline size_t numVecs() const         { return iovecs_.size() - curvecidx_; }
   //! Move forward by numbytes, setting it up so the other functions return the right values.
   void advanceBy(size_t numbytes);

 protected:
   /*!
    * I don't care about chunks, just data (because iovecs are all about data)
    * so do nothing when told about a chunk.
    */
   void use_visitStrChunk(const StrChunkPtr &,
                          const LinearExtent &) override
   {
   }

   /*!
    * Add this new chunk of data to our list.
    */
   void use_visitDataBlock(const void *start, size_t len,
                           const void *, size_t) override
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
   typedef std::vector<iovec> iovecvec;
   StrChunkPtr curchunk_;
   size_t totalbytes_;
   size_t curbyte_;
   size_t curvecidx_;
   iovecvec iovecs_;

   // Private and left undefined on purpose.
   BufferList(const BufferList &b);
   void operator =(const BufferList &b);
};

inline StreamFDModule::BufferList::BufferList()
     : UseTrackingVisitor(true), totalbytes_(0), curbyte_(0), curvecidx_(0)
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
      {
         curchunk_.reset();
      }
      curvecidx_ = 0;
      iovecs_.clear();
      return;
   }

   for (size_t bytestomove = numbytes; bytestomove > 0; )
   {
      iovec &curvec = iovecs_[curvecidx_];
      if (curvec.iov_len <= bytestomove)
      {
         bytestomove -= curvec.iov_len;
         ++curvecidx_;
      }
      else
      {
         curvec.iov_base =
            reinterpret_cast<unsigned char *>(curvec.iov_base) + bytestomove;
         curvec.iov_len -= bytestomove;
         bytestomove = 0;
      }
   }
   curbyte_ += numbytes;
   assert(curvecidx_ < iovecs_.size());
}

//---

struct StreamFDModule::ErrorInfo {
   unievent::EventPtr events_[(ErrFatal - ErrRead) + 1];
   unsigned char errdata_[(ErrFatal - ErrRead) + 1][sizeof(UNIXError)];
   ErrorSet used_;
};

//------

bool StreamFDModule::hasErrorIn(ErrorType err) const noexcept
{
   // ::std::cerr << " ----> Entering StreamFDModule::hasErrorIn(ErrorType err).\n";
   return errorinfo_.used_.test(err);
}

bool StreamFDModule::hasErrorIn(const ErrorSet &set) const noexcept
{
   // ::std::cerr << " ----> Entering StreamFDModule::hasErrorIn(const ErrorSet &set).\n";
   ErrorSet tmp = errorinfo_.used_;
   tmp &= set;
   return tmp.any();
}

void StreamFDModule::onErrorIn(ErrorType err,
                               const unievent::EventPtr &ev) noexcept
{
   // ::std::cerr << " ----> Entering StreamFDModule::onErrorIn().\n";
   errorinfo_.events_[err] = ev;
}

void StreamFDModule::resetErrorIn(ErrorType err) noexcept
{
   // ::std::cerr << " ----> Entering StreamFDModule::resetErrorIn().\n";
   if ((err != ErrFatal) && errorinfo_.used_.test(err))
   {
      reinterpret_cast<UNIXError *>(errorinfo_.errdata_[err])->~UNIXError();
      errorinfo_.used_.reset(err);
      if ((err == ErrGeneral) || (err == ErrRead))
      {
         if (! (errorinfo_.used_.test(ErrGeneral) ||
                errorinfo_.used_.test(ErrRead) ||
                errorinfo_.used_.test(ErrFatal)))
         {
            if ((fd_ >= 0) && !flags_.checkingrd)
            {
               if (!buffed_read_)
               {
                  static const UnixEventRegistry::FDCondSet
                     readset({UnixEventRegistry::FD_Readable});
                  ureg_.registerFDCond(fd_, readset, readev_);
                  flags_.checkingrd = true;
               }
               else
               {
                  disp_.addEvent(resumeread_);
               }
            }
         }
      }
      if ((err == ErrGeneral) || (err == ErrWrite))
      {
         if (! (errorinfo_.used_.test(ErrGeneral) ||
                errorinfo_.used_.test(ErrWrite) ||
                errorinfo_.used_.test(ErrFatal)))
         {
            if ((fd_ >= 0) && !flags_.checkingwr)
            {
               if (cur_write_)
               {
                  static const UnixEventRegistry::FDCondSet
                     writeset({UnixEventRegistry::FD_Writeable});
                  ureg_.registerFDCond(fd_, writeset, writeev_);
                  flags_.checkingwr = true;
               }
               else
               {
                  disp_.addEvent(resumewrite_);
               }
            }
         }
      }
   }
}

const UNIXError &StreamFDModule::getErrorIn(ErrorType err) const noexcept
{
   // ::std::cerr << " ----> Entering StreamFDModule::getErrorIn(ErrorType err).\n";
   if (! errorinfo_.used_.test(err))
   {
      return UNIXError::S_noerror;
   }
   else
   {
      return *(reinterpret_cast<UNIXError *>(errorinfo_.errdata_[err]));
   }
}

void StreamFDModule::setErrorIn(ErrorType err,
                                const unievent::UNIXError &errval)
{
   // ::std::cerr << " ----> Entering StreamFDModule::setErrorIn(ErrorType err, const unievent::UNIXError &errval).\n";
   void *rawmem = errorinfo_.errdata_[err];
   if (errorinfo_.used_.test(err))
   {
      (reinterpret_cast<UNIXError *>(rawmem))->~UNIXError();
   }
   else
   {
      errorinfo_.used_.set(err);
   }
   new(rawmem) UNIXError(errval);
   if (errorinfo_.events_[err])
   {
      disp_.addEvent(errorinfo_.events_[err]);
      {
         errorinfo_.events_[err].reset();
      }
   }
}

size_t StreamFDModule::getBestChunkSize() const
{
   // ::std::cerr << " ----> Entering StreamFDModule::getBestChunkSize().\n";
   static const size_t default_size = 4096U;
   static const size_t smallest = 128U;
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

const StrChunkPtr StreamFDModule::plugRead()
{
   // ::std::cerr << " ----> Entering StreamFDModule::plugRead().\n";
   assert(buffed_read_);

   StrChunkPtr retval;
   retval.swap(buffed_read_);
   doReadFD();
   read_since_read_posted_ += retval->Length();
   if (!buffed_read_ ||
       (read_since_read_posted_ >= S_max_bytes_without_dispatch))
   {
      setReadableFlagFor(&plug_, false);
      if (read_since_read_posted_ >= S_max_bytes_without_dispatch)
      {
         disp_.addEvent(resumeread_);
      }
   }
   return(retval);
}

void StreamFDModule::eventRead()
{
   // ::std::cerr << " ----> Entering StreamFDModule::eventRead().\n";
   assert(!buffed_read_);
   flags_.checkingrd = false;
   read_since_read_posted_ = 0;
   if (!buffed_read_)
   {
      doReadFD();
   }
   if (buffed_read_)
   {
      setReadableFlagFor(&plug_, true);
   }
}

void StreamFDModule::eventResumeRead()
{
   // ::std::cerr << " ----> Entering StreamFDModule::eventResumeRead().\n";
   read_since_read_posted_ = 0;
   if (buffed_read_)
   {
      setReadableFlagFor(&plug_, true);
   }
}

void StreamFDModule::plugWrite(const StrChunkPtr &ptr)
{
   // ::std::cerr << " ----> Entering StreamFDModule::plugWrite().\n";
   assert(!cur_write_);
   assert(ptr);
   cur_write_ = ptr;
   doWriteFD();
   written_since_write_posted_ += ptr->Length();
   if (cur_write_ ||
       (written_since_write_posted_ > S_max_bytes_without_dispatch))
   {
      setWriteableFlagFor(&plug_, false);
      if (written_since_write_posted_ > S_max_bytes_without_dispatch)
      {
         disp_.addEvent(resumewrite_);
      }
   }
}

void StreamFDModule::eventWrite()
{
   // ::std::cerr << " ----> Entering StreamFDModule::eventWrite().\n";
   assert(cur_write_);
   flags_.checkingwr = false;
   written_since_write_posted_ = 0;
   if (cur_write_)
   {
      doWriteFD();
   }
   if (! (hasErrorIn(ErrWrite) ||
          hasErrorIn(ErrGeneral) ||
          hasErrorIn(ErrFatal)))
   {
      if ((fd_ >= 0) && !cur_write_)
      {
         setWriteableFlagFor(&plug_, true);
      }
   }
}

void StreamFDModule::eventResumeWrite()
{
   // ::std::cerr << " ----> Entering StreamFDModule::eventResumeWrite().\n";
   written_since_write_posted_ = 0;
   if (!cur_write_)
   {
      setWriteableFlagFor(&plug_, true);
   }
}

void StreamFDModule::eventError()
{
   // ::std::cerr << " ----> Entering StreamFDModule::eventError().\n";
   setErrorIn(ErrFatal, UNIXError("<none>", EBADF,
                                  LCoreError("Bad FD",
                                             LCORE_GET_COMPILERINFO())));
}

void StreamFDModule::doReadFD()
{
   // ::std::cerr << fd_ << ": In doReadFD\n";
   assert(!buffed_read_);

   if ((fd_ < 0) ||
       hasErrorIn(ErrRead) || hasErrorIn(ErrFatal) || hasErrorIn(ErrGeneral))
   {
      return;
   }

   ssize_t size = -1;
   int myerrno = ESUCCESS;

   {
      // A normal pointer offers a speed advantage, and we don't know whether
      // we want to set buffed_read until the read succeeds.
      const size_t maxsize = getMaxChunkSize();
      ::std::shared_ptr<DynamicBuffer> dbchunk(new DynamicBuffer(maxsize));

      errno = 0;
      // ::std::cerr << "Reading...\n";
      size = ::read(fd_, dbchunk->getVoidP(), maxsize);
      // I may have an error, capture errno to make sure nothing happens to it.
      myerrno = errno;
      // ::std::cerr << fd_ << ": just read " << size << " bytes.\n";

      if (size > 0) {
         dbchunk->resize(size);
         buffed_read_ =  dbchunk;
//  	 ::std::cerr << fd_ << ": just read: <";
//  	 ::std::cerr.write(dbchunk->GetCharP(), dbchunk->Length());
//  	 ::std::cerr << ">\n";
      }
   }

   if (size <= 0)
   {
      // ::std::cerr << "Handling read error.\n";
      assert(!buffed_read_);

      if (size == 0)
      {
         setErrorIn(ErrRead,
                    UNIXError("read", LCoreError(LCORE_GET_COMPILERINFO())));
         // ::std::cerr << fd_ << ": read EOF\n";
	 if (flags_.chunkeof)
	 {
	    buffed_read_ = StrChunkPtr(new EOFStrChunk);
	    // ::std::cerr << fd_ << ": sending EOF chunk\n";
	 }
      }
      else  // size MUST be < 0, and so errno must also be set.
      {
	 // myerrno was set up there right after the read call.
	 if (myerrno == EAGAIN)
         {
            if (!flags_.checkingrd && (fd_ >= 0))
            {
               static const UnixEventRegistry::FDCondSet
                  readset({UnixEventRegistry::FD_Readable});
               ureg_.registerFDCond(fd_, readset, readev_);
               flags_.checkingrd = true;
            }
         }
         else // (myerrno != EAGAIN)
	 {
	    // EAGAIN just means I need to read later, so it's OK, but anything
	    // else isn't.
	    // ::std::cerr << "Handling non-EAGAIN error.\n";
	    // ::std::cerr << fd_ << ": setting ErrRead to " << myerrno << "\n";
            setErrorIn(ErrRead,
                       UNIXError("read", myerrno,
                                 LCoreError(LCORE_GET_COMPILERINFO())));
	 }
      }
   }
   else  // size > 0
   {
      assert(buffed_read_);
   }
}

void StreamFDModule::doWriteFD()
{
   // ::std::cerr << fd_ << ": in doWriteFD()\n";
#ifndef MAXIOVCNT  // UnixWare 7 has this undefined.  *sigh*
#ifndef _SC_IOV_MAX  // Linux has this undefined.  *bigger sigh*
   static const unsigned int MAXIOVCNT = 16;
#else
   static const unsigned int MAXIOVCNT = sysconf(_SC_IOV_MAX);
#endif
#endif

   assert(cur_write_);

   if (fd_ < 0)
   {
      return;
   }

   if (curbuflist_.bytesLeft() <= 0)
   {
      using ::std::dynamic_pointer_cast;
      if (dynamic_pointer_cast<EOFStrChunk, StrChunk>(cur_write_))
      {
         if (!flags_.eofwritten)
         {
            writeEOF();
            // an EOF besides this flag.
            flags_.eofwritten = 1;
         }
         return;
      }
      else
      {
         curbuflist_.startChunk(cur_write_);
      }
   }

   assert(curbuflist_.bytesLeft() > 0);

   // Save the value so compiler can do better optimization
   size_t length = curbuflist_.bytesLeft();

   // This loop is also broken out of when there's an error writing.
   while (length > 0)
   {
      int written;
      size_t numvecs = curbuflist_.numVecs();
      if (numvecs > MAXIOVCNT)
      {
	     numvecs = MAXIOVCNT;
      }
      written = ::writev(fd_, curbuflist_.getIOVec(), numvecs);
      // ::std::cerr << fd_ << ": just wrote: <";
      // ::std::cerr.write(ioveclst.iov[0].iov_base, ioveclst.iov[0].iov_len);
      // ::std::cerr << ">\n";
      // Save errno for later to make sure it isn't clobbered.
      int myerrno = errno;

      if (written < 0)
      {
	 if (myerrno == EAGAIN)
	 {
            if (!flags_.checkingwr && (fd_ >= 0))
            {
               static const UnixEventRegistry::FDCondSet
                  writeset({UnixEventRegistry::FD_Writeable});
               ureg_.registerFDCond(fd_, writeset, writeev_);
               flags_.checkingwr = true;
            }
         }
         else
         {
            setErrorIn(ErrWrite,
                       UNIXError("write", myerrno,
                                 LCoreError(LCORE_GET_COMPILERINFO())));
	 }
	 break;
      }
      else
      {
         curbuflist_.advanceBy(written);
         length -= written;
      }
   }

   assert(length == curbuflist_.bytesLeft());

   if (length <= 0)
   {
      cur_write_.reset();
   }
}

void StreamFDModule::writeEOF()
{
   // ::std::cerr << " ----> Entering StreamFDModule::writeEOF().\n";
   if (fd_ >= 0)
   {
      ::close(fd_);
      ureg_.freeFD(fd_);
      fd_ = -1;
      setErrorIn(ErrWrite,
                  UNIXError("write", LCoreError(LCORE_GET_COMPILERINFO())));
      {
         UNIXError tmp("write", EBADF, LCoreError(LCORE_GET_COMPILERINFO()));
         setErrorIn(ErrFatal, tmp);
         setErrorIn(ErrRead, tmp);
      }
   }
//   ::std::cerr << fd_ << ": setting ErrRead to " << EBADF << "\n";
//     setErrorIn(ErrRead, EBADF);
//     setErrorIn(ErrWrite, EBADF);
//     setErrorIn(ErrFatal, EBADF);
}

StreamFDModule::StreamFDModule(int fd, Dispatcher &disp,
                               UnixEventRegistry &ureg,
			       IOCheckFlags checkmask)
     : fd_(fd),
       plug_(*this),
       curbuflist_(*(new BufferList)),
       max_block_size_(4096),
       read_since_read_posted_(0),
       written_since_write_posted_(0),
       errorinfo_(*(new ErrorInfo)),
       disp_(disp),
       ureg_(ureg)
{
   // ::std::cerr << " ----> Intializing a new StreamFDModule." << std::endl;

   for (unsigned int i = 0;
        i < (sizeof(parenttrackers_) / sizeof(parenttrackers_[0]));
        ++i)
   {
      parenttrackers_[i] = 0;
   }
   setMaxToBest();
   flags_.plugmade = flags_.readeof = flags_.eofwritten = flags_.chunkeof = false;

   // This seems a bit odd, but if you claim to be checking read and write when
   // you really aren't, then the class will never actually register the event
   // with the poll manager that does the read and write checking.
   flags_.checkingrd = flags_.checkingwr = true;

   // Just here to limit the scope of some variables.
   {
      // Assignments done in a careful order to try to be exception safe.
      FDPollRdEv *readev = new FDPollRdEv(*this);
      readev_ = EventPtr(readev);
      FDPollWrEv *writeev = new FDPollWrEv(*this);
      writeev_ = EventPtr(writeev);
      FDPollErEv *errorev = new FDPollErEv(*this);
      errorev_ = EventPtr(errorev);
      ResumeReadEv *rread = new ResumeReadEv(*this);
      resumeread_ = EventPtr(rread);
      ResumeWriteEv *rwrite = new ResumeWriteEv(*this);
      resumewrite_ = EventPtr(rwrite);

      // Set up these pointers so there's a set of things to call the
      // 'parentGone' method on.
      parenttrackers_[0] = readev;
      parenttrackers_[1] = writeev;
      parenttrackers_[2] = errorev;
      parenttrackers_[3] = rread;
      parenttrackers_[4] = rwrite;
   }

   {
      UnixEventRegistry::FDCondSet
         allerrors({UnixEventRegistry::FD_Error,
                    UnixEventRegistry::FD_Closed,
                    UnixEventRegistry::FD_Invalid});
      ureg_.registerFDCond(fd_, allerrors, errorev_);
      // ::std::cerr << " ----> Registering FD error events.\n";
   }
   if (fd_ >= 0)
   {
      if ((checkmask == CheckBoth) || (checkmask == CheckRead))
      {
         // ::std::cerr << " ----> Doing a CheckRead.\n";
         flags_.checkingrd = false;
         doReadFD();
         if (buffed_read_)
         {
            setReadableFlagFor(&plug_, true);
         }
      }
      if ((checkmask == CheckBoth) || (checkmask == CheckWrite))
      {
         // ::std::cerr << " ----> Doing a CheckWrite.\n";
         flags_.checkingwr = false;
         setWriteableFlagFor(&plug_, true);
      }
   }
}

StreamFDModule::~StreamFDModule()
{
   // ::std::cerr << " ----> Destroying an existing StreamFDModule.\n";

   for (unsigned int i = 0;
        i < (sizeof(parenttrackers_) / sizeof(parenttrackers_[0]));
        ++i)
   {
      if (parenttrackers_[i])
      {
         parenttrackers_[i]->parentGone();
         parenttrackers_[i] = 0;
      }
   }
   if (fd_ >= 0)
   {
      ::close(fd_);
      ureg_.freeFD(fd_);
      fd_ = -1;
   }
   for (int i = ErrRead; i <= ErrFatal; ++i)
   {
      ErrorType err = static_cast<ErrorType>(i);
      if (errorinfo_.used_[err])
      {
         (reinterpret_cast<UNIXError *>(errorinfo_.errdata_[i]))->~UNIXError();
         errorinfo_.used_.reset(err);
      }
   }
   delete &curbuflist_;
   delete &errorinfo_;
}

}  // End namespace strmod
}  // End namespace strmod
