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

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif
#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif

#include <UniEvent/EventPtrT.h>
#include <UniEvent/UNIXpollManager.h>
#include <UniEvent/UNIXError.h>

#include <stddef.h>  // size_t

#define _STR_StreamFDModule_H_

class GroupVector;
class UNIXError;

//: This module is for communicating outside your program via UNIX IO.
// <p>One side of this module is a UNIX file descriptor, and the other side is
// the plug.  Everything written to the plug is written to the file
// descriptor, and everything read from the file descriptor is read from the
// plug.</p>
// <p>There are (or will be) ways of asking for events to be posted when stuff
// happens that the StreamFDModule can't deal with directly, such has EOF,
// read or write errors, or unexpected file descriptor closings.</p>
// <p>See StreamModule (the parent class) for descriptions of any member
// functions not described here.</p>
class StreamFDModule : public StreamModule {
 protected:
   class FPlug;
   friend class FPlug;
 private:
   class FDPollEv;
   friend class FDPollEv;
   class FDPollRdEv;
   class FDPollWrEv;
   class FDPollErEv;

 public:
   //: What directions is IO checked in?
   // <p>This is used for file descriptors that are open for only reading, or
   // only writing.</p>
   enum IOCheckFlags { CheckNone, CheckRead, CheckWrite, CheckBoth };
   //: What categories of errors can we have?
   // <p>ErrFatal MUST be the last category for later array declarations to
   // work.</p>
   enum ErrCategory { ErrRead = 0, ErrWrite, ErrOther, ErrFatal };

   static const STR_ClassIdent identifier;

   //: Constructor
   // <code>fd</code> is the file descriptor to attach to.<br>
   // <code>pollmgr</code> is the poll manager to use.  A reference to the
   // poll manager is kept until object destruction.  I don't consider myself
   // to own the poll manager.<br>
   // <code>f</code> talks about the kinds of IO that can be done (and
   // therefore, should be checked for) on <code>fd</code>.<br>
   // <code>hangdelete</code> is whether the last bit of data should be
   // written before close, and whether or not the close should block.<br>
   StreamFDModule(int fd, UNIXpollManager &pollmgr,
		  IOCheckFlags f = CheckBoth, bool hangdelete = true);
   virtual ~StreamFDModule();

   //: See base class Protocol
   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: See base class StreamModule
   inline virtual bool canCreate(int side) const;
   //: See base class StreamModule
   inline virtual bool ownsPlug(const Plug *p) const;
   //: See base class StreamModule
   inline virtual bool deletePlug(Plug *p);

   //: Is this category in an error state?
   inline bool hasErrorIn(ErrCategory ecat) const;
   //: Gets the error that this category has.
   inline const UNIXError getErrorIn(ErrCategory ecat) const;
   //: Sets this category to a non-error state.
   // Doesn't work on the ErrFatal category.
   bool resetErrorIn(ErrCategory ecat);
   //: Do any categories have an error?
   inline bool hasError() const;

   //: Have I read the EOF marker?
   bool readEOF() const                             { return(flags_.readeof); }
   //: Reset the EOF marker so I'll attempt to read more.
   void resetReadEOF();

   bool getSendChunkOnEOF() const                   { return(flags_.chunkeof); }
   inline void setSendChunkOnEOF(bool newval);

   //: Get the file descriptor associated with this module.
   inline int getFD()                               { return(fd_); }

   //: Is the write buffer empty?  Has the module been drained into the fd?
   bool writeBufferEmpty() const                    { return(!cur_write_); }
   //: Is the read buffer empty?  Is the module sucking a currently empty pipe?
   // Note, a permanently empty pipe results in the readEOF() flag being set.
   bool readBufferEmpty() const                     { return(!buffed_read_); }

   //: This returns the optimal IO blocksize for this descriptor
   // <p>This returns the st_blksize member of the stat structure returned by
   // fstat'ing the file descriptor.  If the stat cannot be performed, or
   // st_blksize is 0, a default value (most likely 4096) is returned.</p>
   // <p>In a certain special case, if st_blksize is > 0, but &lt; 64, 64 is
   // returned.</p>
   size_t getBestChunkSize() const;

   //: Sets the maximum block size to be read in a single read operation.
   inline void setMaxChunkSize(size_t mbs);

   //: Mostly equivalent to setMaxChunkSize(BestChunkSize())
   // <p>If the value returned by BestChunkSize is an 'unreasonable' value
   // such as less than 1024 (1k) bytes, or more than 65536 (64k) bytes, the
   // value is set to the closest 'reasonable' value.</p>
   inline void setMaxToBest();

   //: Returns the maximum block size to be read in a single read operation.
   size_t getMaxChunkSize() const                   { return(max_block_size_); }

 protected:
   class MyPollEvent;
   friend class MyPollEvent;
   //: This plug is the rather simplistic plug for a StreamFDModule.
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

   //: See base class.
   virtual const ClassIdent *i_GetIdent() const    { return(&identifier); }

   //: See base class.  Only makes plugs for side 0.
   inline virtual Plug *i_MakePlug(int side);

   //: Called by FPlug::i_Write.
   virtual void plugWrite(const StrChunkPtr &ptr);
   //: Called by FPlug::i_Read.
   virtual const StrChunkPtr plugRead();

   //: Read from fd into buffed_read_
   // <p>Assumes that buffed_read is empty, and various other important
   // things.  UTSL (Use The Source (StrFDPlug.cc) Luke)</p>
   virtual void doReadFD();
   //: Write to fd from cur_write_, using the info in write_vec_.
   // <p>Assumes that cur_write isn't empty, and various other important
   // things.  UTSL (Use The Source (StrFDPlug.cc) Luke)</p>
   virtual void doWriteFD();

   //: An EOF indication has been written.  This function has to handle it.
   // This function follows the template method pattern from Design Patterns.
   // Return true if the fd can now be written to, and return false if it
   // can't.  It would probably also be best to set some kind of error
   // condition if it can't be written to.
   virtual bool writeEOF();

   //: Set an error flag to <code>errnum</code> in the category
   //:<code>ecat</code>.
   void setErrorIn(ErrCategory ecat, int errnum);

   //: Set the flag that says we've read the EOF market to <code>newval</code>.
   void setReadEOF(bool newval);

   //: Check if my UNIXpollManager should tell me if fd_ is readable.
   // <p>Check several different flags and conditions, and if things are
   // alright, ask my UNIXpollManager to post my event when fd_ is writeable.
   // Also set flags_.checkingrd if this is done.</p>
   void maybeShouldReadFD();
   //: Check if my UNIXpollManager should tell me if fd_ is writeable.
   // <p>Check several different flags and conditions, and if things are
   // alright, ask my UNIXpollManager to post an even when fd_ is writeable.
   // Also set flags_.checkingwr if this is done. </p>
   void maybeShouldWriteFD();

   //: Called by readev_'s TriggerEvent.
   void eventRead(unsigned int condbits);
   //: Called by writeev_'s TriggerEvent.
   void eventWrite(unsigned int condbits);
   //: Called by errorev_'s TriggerEvent.
   void eventError(unsigned int condbits);

 private:
   int fd_;
   struct {
      unsigned int plugmade   : 1;
      unsigned int hangdelete : 1;
      unsigned int checkingrd : 1;
      unsigned int checkingwr : 1;
      unsigned int readeof    : 1;
      unsigned int chunkeof   : 1;
   } flags_;
   int errvals[ErrFatal + 1];
   FPlug plug_;
   StrChunkPtr buffed_read_;
   StrChunkPtr cur_write_;
   size_t write_pos_;
   size_t write_length_;
   GroupVector *write_vec_;
   unsigned int max_block_size_;
   FDPollEv *readevptr_;
   UNIEventPtrT<UNIXpollManager::PollEvent> readev_;
   FDPollEv *writeevptr_;
   UNIEventPtrT<UNIXpollManager::PollEvent> writeev_;
   FDPollEv *errorevptr_;
   UNIEventPtrT<UNIXpollManager::PollEvent> errorev_;
   UNIXpollManager &pollmgr_;
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

inline bool StreamFDModule::hasErrorIn(ErrCategory ecat) const
{
   return(errvals[ecat] != 0);
}

inline const UNIXError StreamFDModule::getErrorIn(ErrCategory ecat) const
{
   return(UNIXError(errvals[ecat]));
}

inline bool StreamFDModule::hasError() const
{
   return(hasErrorIn(ErrRead) || hasErrorIn(ErrWrite)
	  || hasErrorIn(ErrOther) || hasErrorIn(ErrFatal));
}

inline void StreamFDModule::setSendChunkOnEOF(bool newval)
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

#endif
