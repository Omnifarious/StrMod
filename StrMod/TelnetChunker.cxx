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
#  pragma implementation "TelnetChunker.h"
#endif

#include "StrMod/TelnetChunker.h"

#ifdef __GNUG__
#  pragma implementation "TelnetChunkerData.h"
#endif

#include "StrMod/TelnetChunkerData.h"
#include "StrMod/TelnetParser.h"
#include "StrMod/TelnetChunkBuilder.h"
#include "StrMod/ChunkIterator.h"
#include "StrMod/DynamicBuffer.h"
#include "StrMod/BufferChunk.h"
#include "StrMod/StrChunkPtr.h"
#include <cstring>
#include <cassert>
#include <iostream>

const STR_ClassIdent TelnetChunker::identifier(33UL);
const STR_ClassIdent TelnetChunker::TelnetData::identifier(34UL);
const STR_ClassIdent TelnetChunker::SingleChar::identifier(35UL);
const STR_ClassIdent TelnetChunker::Suboption::identifier(36UL);
const STR_ClassIdent TelnetChunker::OptionNegotiation::identifier(37UL);

const U1Byte TelnetChunker::Suboption::optend_[2] = {
   TelnetChars::IAC, TelnetChars::SE
};

class TelnetChunker::Builder : public TelnetChunkBuilder {
 private:
   static const int qsize_ = 16;
 public:
   Builder();
   virtual ~Builder();

   virtual void addCharacter(U1Byte c);
   virtual void addIACAndCharacter(U1Byte c);
   virtual void addCharCommand(TelnetChars::Commands command)
      throw(bad_call_order);
   virtual void addNegotiationCommand(TelnetChars::OptionNegotiations negtype,
                                      U1Byte opt_type)
      throw(bad_call_order);
   virtual bool isInSuboption() const             { return insubopt_; }
   virtual void initSuboption(U1Byte opt_type) throw(bad_call_order);
   virtual void finishSuboption() throw(bad_call_order);

   bool queueFull() const                         { return queueend_ < qsize_; }
   inline bool queueEmpty() const                 { return queueend_ == 0; }
   inline bool hasData() const;
   StrChunkPtr getFromQueue();

 private:
   StrChunk *chunks_[qsize_];
   int queueend_;
   BufferChunk *curdata_;
   U1Byte *buf_;
   size_t used_;
   size_t len_;
   bool insubopt_;
   U1Byte suboptnum_;

   inline void newBuffer(bool queuecurrent = true);
};

inline bool
TelnetChunker::Builder::hasData() const
{
   return (queueend_ != 0) || (!insubopt_ && (used_ != 0));
}

inline void TelnetChunker::Builder::newBuffer(bool queuecurrent)
{
   BufferChunk *cd = curdata_;
   if (queuecurrent && (used_ > 0))
   {
      cd->resize(used_);
      chunks_[queueend_++] = cd;
   }
   cd = new DynamicBuffer(8);
   buf_ = cd->getCharP();
   len_ = 8;
   used_ = 0;
   curdata_ = cd;
}

TelnetChunker::Builder::Builder()
     : queueend_(0), curdata_(0), buf_(0), used_(0), len_(0), insubopt_(false)
{
   for (int i = 0; i < qsize_; ++i)
   {
      chunks_[i] = 0;
   }
   newBuffer(false);
}

void TelnetChunker::Builder::addCharacter(U1Byte c)
{
   if (used_ >= len_)
   {
      // atleast must be a power of 2.
      static const size_t atleast = 0x08;
      static const size_t mask = ~static_cast<size_t>(atleast - 1);
      BufferChunk *cd = curdata_;
      cd->resize(used_ + ((used_ / 2) & ~mask) + atleast);
      len_ = cd->Length();
      buf_ = cd->getCharP();
   }
   buf_[used_++] = c;
}

void TelnetChunker::Builder::addIACAndCharacter(U1Byte c)
{
   addCharacter(TelnetChars::IAC);
   addCharacter(c);
}

void TelnetChunker::Builder::addCharCommand(TelnetChars::Commands command)
      throw(bad_call_order)
{
   if (insubopt_)
   {
      throw bad_call_order("TelnetChunker::Builder::addCharCommand called "
                           "while still inside a suboption.");
   }
   if (used_ > 0)
   {
      newBuffer();
   }
   chunks_[queueend_++] = new SingleChar(command);
}

void TelnetChunker::Builder::addNegotiationCommand(
   TelnetChars::OptionNegotiations negtype, U1Byte opt_type)
   throw(bad_call_order)
{
   if (insubopt_)
   {
      throw bad_call_order("TelnetChunker::Builder::addNegotiationCommand "
                           "called while still inside a suboption.");
   }
   if (used_ > 0)
   {
      newBuffer();
   }
   chunks_[queueend_++] = new OptionNegotiation(negtype, opt_type);
}

void TelnetChunker::Builder::initSuboption(U1Byte opt_type)
   throw(bad_call_order)
{
   if (insubopt_) {
      throw bad_call_order("TelnetChunker::Builder::initSuboption "
                           "called while still inside a suboption.");
   }
   suboptnum_ = opt_type;
   insubopt_ = true;
   if (used_ > 0)
   {
      newBuffer();
   }
}

void TelnetChunker::Builder::finishSuboption()
   throw(bad_call_order)
{
   if (!insubopt_) {
      throw bad_call_order("TelnetChunker::Builder::finishSuboption "
                           "called while not inside a suboption.");
   }
   curdata_->resize(used_);
   chunks_[queueend_++] = new Suboption(suboptnum_, curdata_);
   newBuffer(false);
   insubopt_ = false;
}

StrChunkPtr
TelnetChunker::Builder::getFromQueue()
{
   StrChunk *ptr = 0;
   if (queueend_ > 0)
   {
      ptr = chunks_[0];
      queueend_--;
      for (int i = 1; i < queueend_; ++i)
      {
         chunks_[i - 1] = chunks_[i];
      }
      chunks_[queueend_] = 0;
   }
   else if (!insubopt_ && (used_ > 0))
   {
      curdata_->resize(used_);
      ptr = curdata_;
      newBuffer(false);
   }
   return(ptr);
}

struct TelnetChunker::Internals {
   TelnetParser parser_;
   Builder builder_;
};

TelnetChunker::TelnetChunker()
     : data_(*(new Internals))
{
}

TelnetChunker::~TelnetChunker()
{
   delete &data_;
}

void TelnetChunker::processIncoming()
{
   if (! chunkpos_.isFor(incoming_))
   {
      chunkpos_ = incoming_->begin();
   }
   StrChunk::const_iterator end = incoming_->end();
   for (; chunkpos_ != end; ++chunkpos_)
   {
      data_.parser_.processChar(*chunkpos_, data_.builder_);
   }
}

void TelnetChunker::SingleChar::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   call_visitDataBlock(visitor, buf_, 2);
}

TelnetChunker::Suboption::Suboption(U1Byte type,
                                    const StrChunkPtrT<BufferChunk> &cooked)
     : raw_(cookedToRaw(cooked)), rawlen_(raw_->Length()), cooked_(cooked)
{
   optstart_[0] = TelnetChars::IAC;
   optstart_[1] = TelnetChars::SB;
   optstart_[2] = type;
}

const StrChunkPtr
TelnetChunker::Suboption::cookedToRaw(const StrChunkPtrT<BufferChunk> &cooked)
{
   const U1Byte * const cookedbuf = cooked->getCharP();
   const size_t cookedlen = cooked->Length();
   unsigned int countIAC = 0;

   for (size_t i = 0; i < cookedlen; ++i)
   {
      if (cookedbuf[i] == TelnetChars::IAC)
      {
         ++countIAC;
      }
   }
   if (countIAC == 0)
   {
      return(cooked);
   }
   else
   {
      // Every IAC currently in cooked will be replaced with IAC IAC.
      size_t rawlen = cooked->Length() + countIAC;
      DynamicBuffer *raw = new DynamicBuffer(rawlen);
      {
         U1Byte * const rawbuf = raw->getCharP();
         size_t o = 0;

         for (size_t i = 0; i < cookedlen; ++i)
         {
            U1Byte curchar = cookedbuf[i];
            if (curchar == TelnetChars::IAC)
            {
               rawbuf[o++] = curchar;
               rawbuf[o++] = curchar;
            }
            else
            {
               rawbuf[o++] = curchar;
            }
         }
         assert(o == rawlen);
      }
      return(raw);
   }
}

void TelnetChunker::Suboption::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   call_visitDataBlock(visitor, optstart_, 3);
   call_visitStrChunk(visitor, raw_);
   call_visitDataBlock(visitor, const_cast<unsigned char *>(optend_), 2);
}

void TelnetChunker::OptionNegotiation::acceptVisitor(ChunkVisitor &visitor)
   throw(ChunkVisitor::halt_visitation)
{
   call_visitDataBlock(visitor, buf_, 3);
}
