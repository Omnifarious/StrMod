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
#include "StrMod/StrSubChunk.h"
#include "StrMod/GroupChunk.h"
#include "StrMod/PreAllocBuffer.h"
#include "StrMod/DynamicBuffer.h"
#include "StrMod/ApplyVisitor.h"
#include "StrMod/StrChunkPtrT.h"
#include "StrMod/StrChunkPtr.h"
#include <cassert>
#include <deque>

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
   Builder() : curlen_(0)                                   { }
   virtual ~Builder()                                       { }

   virtual void addDataBlock(size_t regionbegin, size_t regionend);
   virtual void addCharCommand(TelnetChars::Commands command);
   virtual void addNegotiationCommand(TelnetChars::OptionNegotiations negtype,
                                      U1Byte opt_type);
   virtual void addSuboption(U1Byte opt_type,
                             size_t regionbegin, size_t regionend,
                             StrChunkPtrT<BufferChunk> &cooked);

   void addIncoming(const StrChunkPtr &ptr);
   size_t curIncomingLen() const                            { return curlen_; }
   inline void clearIncoming();
   inline void setIncoming(const StrChunkPtr &ptr);
   inline void setIncoming(const StrChunkPtr &ptr, size_t inlen);

   inline bool hasData() const;
   const StrChunkPtr getFromQueue();

 private:
   deque<StrChunk *> chunks_;
   StrChunkPtr curchunk_;
   size_t curlen_;

   inline StrChunk *makeDataPtr(size_t regionbegin, size_t regionend);
};

inline void TelnetChunker::Builder::clearIncoming()
{
   curchunk_.ReleasePtr();
   curlen_ = 0;
}

inline void TelnetChunker::Builder::setIncoming(const StrChunkPtr &ptr)
{
   setIncoming(ptr, ptr->Length());
}

inline void
TelnetChunker::Builder::setIncoming(const StrChunkPtr &ptr, size_t inlen)
{
   curchunk_ = ptr;
   curlen_ = inlen;
}

inline bool TelnetChunker::Builder::hasData() const
{
   return !chunks_.empty();
}

inline StrChunk *
TelnetChunker::Builder::makeDataPtr(size_t regionbegin, size_t regionend)
{
   if ((regionbegin == 0) && (regionend == curlen_))
   {
      return curchunk_.GetPtr();
   }
   else
   {
      return new StrSubChunk(curchunk_,
                             LinearExtent(regionbegin,
                                          regionend - regionbegin));
   }
}

void TelnetChunker::Builder::addDataBlock(size_t regionbegin, size_t regionend)
{
   assert(curchunk_);
   assert(regionend >= regionbegin);
   assert(regionbegin <= curlen_);
   assert(regionend <= curlen_);
   if (regionend == regionbegin)
   {
      return;
   }
   StrChunk *ptr = makeDataPtr(regionbegin, regionend);
   ptr->AddReference();
   chunks_.push_back(ptr);
}

void TelnetChunker::Builder::addCharCommand(TelnetChars::Commands command)
{
   StrChunk *ptr = new SingleChar(command);
   ptr->AddReference();
   chunks_.push_back(ptr);
}

void TelnetChunker::Builder::addNegotiationCommand(
   TelnetChars::OptionNegotiations negtype, U1Byte opt_type)
{
   StrChunk *ptr = new OptionNegotiation(negtype, opt_type);
   ptr->AddReference();
   chunks_.push_back(ptr);
}

void TelnetChunker::Builder::addSuboption(U1Byte opt_type,
                                          size_t regionbegin, size_t regionend,
                                          StrChunkPtrT<BufferChunk> &cooked)
{
   assert(curchunk_);
   assert(regionend > regionbegin);
   assert(regionbegin < curlen_);
   assert(regionend <= curlen_);
   StrChunk *data = 0;
   if (regionend > regionbegin)
   {
      data = makeDataPtr(regionbegin, regionend);
   }
   StrChunk *ptr = new Suboption(opt_type, cooked, data);
   ptr->AddReference();
   chunks_.push_back(ptr);
}

void TelnetChunker::Builder::addIncoming(const StrChunkPtr &ptr)
{
   const size_t inlen = ptr->Length();
   const size_t oldlen = curlen_;
   assert(inlen > 0);
   if (inlen == 0)
   {
      return;
   }
   assert((curchunk_ && (curlen_ > 0)) || (!curchunk_ && (curlen_ == 0)));
   if (!curchunk_)
   {
      setIncoming(ptr, inlen);  // Also sets curlen_
   }
   else
   {
      StrChunkPtrT<GroupChunk> gc;
      if ((curchunk_->NumReferences() == 1) &&
          curchunk_->AreYouA(GroupChunk::identifier))
      {
         gc = static_cast<GroupChunk *>(curchunk_.GetPtr());
      }
      else
      {
         GroupChunk *tmp = new GroupChunk;
         tmp->push_back(curchunk_);
         gc = tmp;
         curchunk_ = tmp;
      }
      gc->push_back(ptr);
      curlen_ += inlen;
      assert(curlen_ == curchunk_->Length());
   }
   assert(curlen_ == (oldlen + inlen));
}

const StrChunkPtr TelnetChunker::Builder::getFromQueue()
{
   assert(!chunks_.empty());
   if (!chunks_.empty())
   {
      StrChunk *ptr = chunks_.front();
      chunks_.pop_front();
      assert(ptr->NumReferences() > 0);
      ptr->DelReference();
      return(ptr);
   }
   else
   {
      return 0;
   }
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

class TelnetChunker::DataFunctor {
 public:
   DataFunctor(TelnetParser &parser, TelnetChunkBuilder &builder)
        : parser_(parser), builder_(builder)
   {
   }
   void operator ()(const void *data, size_t len) const {
      parser_.processData(data, len, builder_);
   }

   TelnetParser &parser_;
   TelnetChunkBuilder &builder_;
};

void TelnetChunker::processIncoming()
{
   if (!data_.builder_.hasData())
   {
      if (incoming_->Length() <= 0)
      {
         incoming_.ReleasePtr();
         return;
      }

      data_.builder_.addIncoming(incoming_);
      {
         DataFunctor tmp(data_.parser_, data_.builder_);
         for_each(incoming_, tmp);
      }
      data_.parser_.endOfChunk(data_.builder_);

      const size_t regionbegin = data_.parser_.getRegionBegin();
      const size_t totallen = data_.builder_.curIncomingLen();
      assert(regionbegin <= totallen);
      // If the parser doesn't forsees a region starting before the end of the
      // current block.
      if (regionbegin == totallen)
      {
         // Drop
         data_.parser_.dropBytes(regionbegin);
         data_.builder_.clearIncoming();
      }
      else
      {
         const size_t inlen = incoming_->Length();

         assert(totallen >= inlen);
         // If the parser doesn't forsee a region starting before the beginning
         // of the block we just processed, drop all prior blocks.
         if ((totallen == inlen) && ((totallen - regionbegin) <= inlen))
         {
            data_.parser_.dropBytes(totallen - inlen);
            data_.builder_.setIncoming(incoming_, inlen);
         }
      }
   }
   assert(data_.builder_.hasData());;
   if (data_.builder_.hasData())
   {
      outgoing_ = data_.builder_.getFromQueue();
      outgoing_ready_ = true;
   }
   if (!data_.builder_.hasData())
   {
      incoming_.ReleasePtr();
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
      BufferChunk *raw = 0;
      if (rawlen <= 16)
      {
         raw = new PreAllocBuffer<16>;
         raw->resize(rawlen);
      }
      else
      {
         raw = new DynamicBuffer(rawlen);
      }
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
