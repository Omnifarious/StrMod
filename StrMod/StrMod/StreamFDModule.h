#ifndef _STR_StreamFDModule_H_  //-*-c++-*-

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

// See ../ChangLog for log.
// $Revision$

//! author="Eric Hopper" lib=StrMod

#include <cstddef>  // size_t

#include <UniEvent/EventPtr.h>
#include <UniEvent/EventPtrT.h>
#include <UniEvent/UnixEventRegistry.h>
#include <UniEvent/UNIXError.h>

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif
#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif

#define _STR_StreamFDModule_H_

template <class enum_t, enum_t first, enum_t last> class enum_set;

namespace strmod {
namespace strmod {

class GroupVector;

/** \class StreamFDModule StreamFDModule.h StrMod/StreamFDModule.h
 * \brief This module is for communicating outside your program via UNIX IO.
 *
 * One side of this module is a UNIX file descriptor, and the other side is the
 * plug.  Everything written to the plug is written to the file descriptor, and
 * everything read from the file descriptor is read from the plug.
 *
 * There are (or will be) ways of asking for events to be posted when stuff
 * happens that the StreamFDModule can't deal with directly, such has EOF,
 * read or write errors, or unexpected file descriptor closings.
 */
class StreamFDModule : public StreamModule {
 protected:
   class FPlug;
   friend class FPlug;
 private:
   class BufferList;
   class EvMixin;
   friend class EvMixin;
   class FDPollRdEv;
   friend class FDPollRdEv;
   class FDPollWrEv;
   friend class FDPollWrEv;
   class FDPollErEv;
   friend class FDPollErEv;
   class ResumeReadEv;
   friend class ResumeReadEv;
   class ResumeWriteEv;
   friend class ResumeWriteEv;

 public:
   /** What directions is IO checked in?
    * This is used for file descriptors that are open only for reading, or only
    * for writing.
    */
   enum IOCheckFlags {
      CheckNone,  //!< Don't bother with checking for reading or writing.
      CheckRead,  //!< Only check file descriptor for reading.
      CheckWrite, //!< Only check file descriptor for writing.
      CheckBoth   //!< Check file descriptor for reading and writing.
   };
   /** What kinds of errors are possible?
    */
   enum ErrorType {
      ErrRead,  //!< Error while reading, might have read an EOF.  Must be lowest enum value.
      ErrWrite, //!< Error while writing, might have written an EOF.
      ErrGeneral,  //!< General error affecting both reading and writing.
      ErrFatal  //! General, fatal error affecting both reading and writing.  Must be highest enum value.
   };
   //! Typedef for set of error types.
   typedef ::enum_set<ErrorType, ErrRead, ErrFatal> ErrorSet;


   static const STR_ClassIdent identifier;
   /** \brief This is the maximum number of bytes to read or write without going
    * back to the dispatcher.
    *
    * If the socket on this class has enough data going through it to saturate
    * the CPU, the StreamFDModule may never give the CPU to any of the other
    * StreamFDModules unless it voluntarily gives it up after passing a certain
    * amount of data.  This is the constant that determines how much data is
    * enough.
    *
    * This should possibly be a configurable value, but it isn't yet.  If it is
    * too low, you call back the dispatcher too often, and high bandwidth
    * connections are handled slightly less efficiently than they could be.  If
    * it's too high you'll get choppy response.  It's kinda like the
    * multitasking timeslice value in an OS.
    */
   static const size_t S_max_bytes_without_dispatch = 256U * 1024U;

   /** Constructs a StreamFDModule from an OS file descriptor.
    *
    * @param fd The file descriptor to attach to.
    *
    * @param disp The strmod::unievent::Dispatcher to use for posting an event
    * to when it becomes apparent that this module is in danger of hogging the
    * CPU.
    *
    * @param pollmgr The poll manager to use.  A reference to the poll manager
    * is kept until object destruction.  The StreamFDModule doesn't 'own' the
    * poll manager.
    *
    * @param checkmask Describes what kinds of IO that can be done (and
    * therefore, should be checked for) on <code>fd</code>.
    */
   StreamFDModule(int fd,
                  unievent::Dispatcher &disp,
                  unievent::UnixEventRegistry &ureg,
		  IOCheckFlags checkmask = CheckBoth);
   //! Closes the associated file descriptor.
   virtual ~StreamFDModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool canCreate(int side) const;
   inline virtual bool ownsPlug(const Plug *p) const;
   inline virtual bool deletePlug(Plug *p);

   //! Check for an error in the given category.
   bool hasErrorIn(ErrorType err) const throw ();
   //! Check for an error in one of the categories given by the set.
   bool hasErrorIn(const ErrorSet &set) const throw ();
   /** Ask to post an event when the given error type happens.
    * Currently only one event per error type is allowed.  If there is already
    * an event for a particular error type, that event will be forgotten about
    * and not posted.
    */
   void onErrorIn(ErrorType err, const unievent::EventPtr &ev) throw();
   /** Reset the error value for a particular category.
    * This does not work for the ErrFatal category.
    */
   void resetErrorIn(ErrorType err) throw ();
   //! Get the error value for a particular category.
   const unievent::UNIXError &getErrorIn(ErrorType err) const throw ();

   //! Set whether or not and EOFStrChunk is sent when an EOF is read.
   inline void setSendChunkOnEOF(bool newval) throw();
   //! Does this module send a chunk on EOF?
   inline bool getSendChunkOnEOF() throw()          { return flags_.chunkeof; }

   //! Sets the maximum block size to be read in a single read operation.
   inline void setMaxChunkSize(size_t mbs);
   //! Returns the maximum block size to be read in a single read operation.
   size_t getMaxChunkSize() const                   { return(max_block_size_); }

   /** This returns the optimal IO blocksize for this descriptor
    *
    * This returns the st_blksize member of the stat structure returned by
    * fstat'ing the file descriptor.  If the stat cannot be performed, or
    * st_blksize is 0, a default value (most likely 4096) is returned.
    *
    * In a certain special case, if 0 < st_blksize < 64, 64 is returned.
    */
   size_t getBestChunkSize() const;

   /** Mostly equivalent to setMaxChunkSize(BestChunkSize())
    * If the value returned by BestChunkSize is an 'unreasonable' value such as
    * less than 1024 (1k) bytes, or more than 65536 (64k) bytes, the value is
    * set to the closest 'reasonable' value.
    */
   inline void setMaxToBest();


 protected:
   class MyPollEvent;
   friend class MyPollEvent;
   //: This plug is the rather simple plug for a StreamFDModule.
   class FPlug : public Plug {
      friend class StreamFDModule;
    public:
      static const STR_ClassIdent identifier;

      //: Note that this can ONLY be constructed using a StreamFDModule.
      FPlug(StreamFDModule &parent) : Plug(parent)      { }
      virtual ~FPlug()                                  { }

      //: See base class.
      inline virtual int AreYouA(const ClassIdent &cid) const;

      //: Grab Plug::getParent, and cast it to the real type of the parent.
      inline StreamFDModule &getParent() const;

      //: This plug is always on side 0.
      virtual int side() const                          { return(0); }

    protected:
      //: See base class.
      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      //: Forwards to getParent()->plugRead()
      inline virtual const StrChunkPtr i_Read();
      //: Forwards to getParent()->plugWrite()
      inline virtual void i_Write(const StrChunkPtr &ptr);
   };

   virtual const ClassIdent *i_GetIdent() const    { return(&identifier); }

   inline virtual Plug *i_MakePlug(int side);

   //! Called by FPlug::i_Write.
   virtual void plugWrite(const StrChunkPtr &ptr);
   //! Called by FPlug::i_Read.
   virtual const StrChunkPtr plugRead();

   /** Read from fd into buffed_read_
    *
    * Assumes that buffed_read_ is empty, that there are no read errors, no
    * fatal errors, and that EOF has not been read.
    *
    * If the read returns '0' and the EOF on read flag is set, buffed_read_ will
    * contain an EOFStrChunk.
    */
   virtual void doReadFD();
   /** Write to fd from cur_write_, using the info in write_vec_.
    *
    * Assumes that cur_write_ isn't empty, that there are no write errors, no
    * fatal errors, and that EOF has not been written.
    *
    * If cur_write_ is an EOFStrChunk, it will call writeEOF().
    */
   virtual void doWriteFD();

   /** An EOF indication has been written.  This function has to handle it.
    * This function follows the template method pattern from Design Patterns.
    *
    * Do whatever is needed to write an EOF to the file descriptor for
    * this module.
    */
   virtual void writeEOF();

   //! Set an error in a particular category.
   void setErrorIn(ErrorType err, const unievent::UNIXError &errval);

   //! Called by readev_ and resumeread_'s triggerEvent.
   void eventRead();
   //! Called by writeev_ and resumewrite_'s triggerEvent.
   void eventWrite();
   //! Called by errorev_'s triggerEvent.
   void eventError();
   //! Called by resumeread_'s triggerEvent
   void eventResumeRead();
   //! Called by resumewrite_'s triggerEvent
   void eventResumeWrite();

   //! Get the file descriptor so a derived class can do something to it.
   int getFD() const throw()                       { return fd_; }

 private:
   struct ErrorInfo;

   int fd_;
   struct {
      unsigned int plugmade   : 1;
      unsigned int checkingrd : 1;
      unsigned int checkingwr : 1;
      unsigned int readeof    : 1;
      unsigned int eofwritten : 1;
      unsigned int chunkeof   : 1;
   } flags_;
//   int errvals[ErrFatal + 1];
   FPlug plug_;
   StrChunkPtr buffed_read_;
   StrChunkPtr cur_write_;
   BufferList &curbuflist_;
   unsigned int max_block_size_;
   unsigned int read_since_read_posted_;
   unsigned int written_since_write_posted_;
   // Non reference counted pointers, used for housekeeping.
   EvMixin *parenttrackers_[5];
   // The following 5 members are duplicates of the pointers held in the
   // previous array.  They are reference counted.
   typedef unievent::EventPtr EventPtr;
   EventPtr readev_;
   EventPtr writeev_;
   EventPtr errorev_;
   unievent::EventPtr resumeread_;
   unievent::EventPtr resumewrite_;
   ErrorInfo &errorinfo_;
   unievent::Dispatcher &disp_;
   unievent::UnixEventRegistry &ureg_;
};

//-----------------inline functions for class StreamFDModule-------------------

inline int StreamFDModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool StreamFDModule::canCreate(int side) const
{
   return (side == 0 && !flags_.plugmade);
}

inline bool StreamFDModule::ownsPlug(const Plug *p) const
{
   return(flags_.plugmade && (p == &plug_));
}

inline bool StreamFDModule::deletePlug(Plug *p)
{
   if (ownsPlug(p)) {
      flags_.plugmade = 0;
      return(true);
   } else
      return(false);
}

//  inline bool StreamFDModule::hasErrorIn(ErrCategory ecat) const
//  {
//     return(errvals[ecat] != 0);
//  }

//  inline const UNIXError StreamFDModule::getErrorIn(ErrCategory ecat) const
//  {
//     return(UNIXError(errvals[ecat]));
//  }

//  inline bool StreamFDModule::hasError() const
//  {
//     return(hasErrorIn(ErrRead) || hasErrorIn(ErrWrite)
//  	  || hasErrorIn(ErrOther) || hasErrorIn(ErrFatal));
//  }

inline void StreamFDModule::setSendChunkOnEOF(bool newval) throw()
{
   flags_.chunkeof = newval;
}

inline void StreamFDModule::setMaxChunkSize(size_t mbs)
{
   assert(mbs > 0);

   if (mbs > 0)
   {
      max_block_size_ = mbs;
   }
}

inline void StreamFDModule::setMaxToBest()
{
   setMaxChunkSize(getBestChunkSize());
}

inline StreamModule::Plug *StreamFDModule::i_MakePlug(int side)
{
   if (canCreate(side))
   {
      flags_.plugmade = 1;
      return(&plug_);
   }
   else
   {
      return(0);
   }
}

//-------------inline functions for class StreamFDModule::FPlug----------------

inline int StreamFDModule::FPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline StreamFDModule &StreamFDModule::FPlug::getParent() const
{
   return(static_cast<StreamFDModule &>(Plug::getParent()));
}

inline const StrChunkPtr StreamFDModule::FPlug::i_Read()
{
   return(getParent().plugRead());
}

inline void StreamFDModule::FPlug::i_Write(const StrChunkPtr &ptr)
{
   getParent().plugWrite(ptr);
}

}  // namespace strmod
}  // namespace strmod

#endif
