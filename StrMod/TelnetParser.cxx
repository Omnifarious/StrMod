/*
 * Copyright 2001 by Eric M. Hopper <hopper@omnifarious.mn.org>
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
#  pragma implementation "TelnetParser.h"
#endif

#include "StrMod/TelnetParser.h"
#include "StrMod/TelnetChunkBuilder.h"
#include "StrMod/TelnetChars.h"
#include "StrMod/LocalCopy.h"
#include "StrMod/PreAllocBuffer.h"
#include "StrMod/StrChunkPtrT.h"
#include <cassert>
#include <cstddef>

TelnetParser::TelnetParser()
     : curpos_(0), state_(PS_Normal), regionbegin_(0), regionend_(0)
{
}

inline void TelnetParser::stateNormal(ParserState &state, U1Byte ch)
{
   if (ch == TelnetChars::IAC)
   {
      state = PS_Escape;
   }
}

inline void TelnetParser::stateEscape(ParserState &state,
                                      const U1Byte ch, size_t i,
                                      TelnetChunkBuilder &builder)
{
   if (ch == TelnetChars::IAC)
   {
      regionend_ = i;
      // Up to, but not including current character.
      builder.addDataBlock(regionbegin_, regionend_);
      regionbegin_ = i + 1;  // Skip current character.
      state = PS_Normal;
   }
   if (ch == TelnetChars::SB)
   {
      state = PS_SuboptNum;
   }
   else if (TelnetChars::convertCharToOptionNegotiation(ch, negtype_))
   {
      state = PS_SubNeg;
   }
   else
   {
      TelnetChars::Commands cmd;

      if (TelnetChars::convertCharToCommand(ch, cmd))
      {
         // This character, nor the previous character is to be included.
         regionend_ = i - 1;
         assert(regionend_ >= regionbegin_);
         if (regionend_ > regionbegin_)
         {
            builder.addDataBlock(regionbegin_, regionend_);
         }
         builder.addCharCommand(cmd);  // Add what was just found.
         // Region starts after the single character command.
         regionbegin_ = i + 1;
         state = PS_Normal;
      }
      else  // A silly escape came in.
      {
         state = PS_Normal;
         // As a kludge, take the escape and following character
         // literally, which basically means to extend the current region
         // over them.
      }
   }
}

inline void TelnetParser::stateSubNeg(ParserState &state,
                                      const U1Byte ch, size_t i,
                                      TelnetChunkBuilder &builder)
{
   // <IAC> { <WILL>, <WONT>, <DO>, <DONT> } <type>, have been process
   // and it's currently pointing at <type>, so the previous region ends
   // two characters back.
   regionend_ = i - 2;
   assert(regionend_ >= regionbegin_);
   if (regionend_ > regionbegin_)
   {
      builder.addDataBlock(regionbegin_, regionend_);
   }
   builder.addNegotiationCommand(negtype_, ch);
   state = PS_Normal;
}

inline void TelnetParser::stateSuboptNum(ParserState &state,
                                         const U1Byte ch, size_t i,
                                         TelnetChunkBuilder &builder)
{
   // <IAC> <SB> <type>, have been process and it's currently pointing
   // at <type>, so the previous region ends two characters back.
   regionend_ = i - 2;
   assert(regionend_ >= regionbegin_);
   assert(curpos_ >= regionbegin_);
   if (regionend_ > regionbegin_)
   {
      builder.addDataBlock(regionbegin_, regionend_);
   }
   if (ch != 255) // This doesn't handle the extended suboption (RFC 861)
   {
      // Handle all but the extended suption.
      subopt_type_ = ch;
      cooked_ = new PreAllocBuffer<48>;
      cooked_->resize(48);
      cookedbuf_ = cooked_->getCharP();
      cooked_total_ = 48;
      cooked_used_ = 0;
      state = PS_Subopt;
   }
   else  // Do _something_ about the extended suboption.
   {
      // This shouldn't happen.  Telnet engines using this parser should
      // always say 'WONT' and 'DONT' to the extended suboption option.
      // Some sort of reset anyway.  This will result in the offending
      // <IAC> <SB> <type> sequence being ignored.
      state = PS_Normal;
   }
   // The next region always begins after the <IAC> <SB> <type> sequence
   // so point regionbegin_ to its beginning.
   regionbegin_ = i + 1;
}

inline void TelnetParser::stateSubopt(ParserState &state, U1Byte ch)
{
   if (ch == TelnetChars::IAC)
   {
      state = PS_SuboptEscape;
   }
   else
   {
      if (cooked_used_ >= cooked_total_)
      {
         cooked_->resize(cooked_used_ + cooked_used_ / 2);
         cookedbuf_ = cooked_->getCharP();
         cooked_total_ = cooked_->Length();
         assert(cooked_total_ > cooked_used_);
      }
      cookedbuf_[cooked_used_++] = ch;
   }
}

inline void TelnetParser::stateSuboptEscape(ParserState &state,
                                            U1Byte ch, size_t i,
                                            TelnetChunkBuilder &builder)
{
   if (ch == TelnetChars::SE)
   {
      // It's read in <IAC> <SE> and currently pointing at SE.  The end
      // of the region of option data is just before the <IAC>, so one
      // character back, since we take end as being 'the character just
      // after the last desired character' ala STL.
      regionend_ = i - 1;
      cooked_->resize(cooked_used_);
      {
         StrChunkPtrT<BufferChunk> tmp(cooked_);
         builder.addSuboption(subopt_type_, regionbegin_, regionend_, tmp);
      }
      cooked_ = 0;
      regionbegin_ = i + 1;
      state = PS_Normal;
   }
   else
   {
      state = PS_Subopt;
      if ((cooked_used_ + 1) >= cooked_total_)
      {
         cooked_->resize(cooked_used_ + cooked_used_ / 2);
         cookedbuf_ = cooked_->getCharP();
         cooked_total_ = cooked_->Length();
         assert(cooked_total_ > (cooked_used_ + 1));
      }
      if (ch != TelnetChars::IAC)
      {
         cookedbuf_[cooked_used_++] = TelnetChars::IAC;
      }
      cookedbuf_[cooked_used_++] = ch;
   }
}

/**
 * \param data The data buffer to be stuffed into the state machine.
 * \param len The number of bytes in the data buffer.
 * \param builder The TelnetChunkBuilder to use to build the parsed objects.
 *
 * Run the internal state machine on a buffer of characters.  The internal state
 * machine will call the appropriate TelnetChunkBuilder methods.
 *
 * This allows the internal state machine to only pay attention to the
 * character sequence and be largely divorced from how the characters are
 * actually sliced and diced.
 *
 * As a courtesy to slicers and dicers, it will keep track of a region, and
 * details about certain kinds of parsed objects.
 */
void TelnetParser::processData(const void *data, size_t len,
                               TelnetChunkBuilder &builder)
{
   assert((state_ == PS_Normal) || (state_ == PS_Escape)
	  || (state_ == PS_SubNeg) || (state_ == PS_SuboptNum)
	  || (state_ == PS_Subopt) || (state_ == PS_SuboptEscape));


   const U1Byte *chars = static_cast<const U1Byte *>(data);
   const size_t dataend = curpos_ + len;
   LocalCopy<ParserState> state(state_);

   for (size_t i = curpos_; i < dataend; ++i)
   {
      const U1Byte ch = *chars++;
      switch (state.local)
      {
       case PS_Normal:
         stateNormal(state.local, ch);
         break;
       case PS_Escape:
         stateEscape(state.local, ch, i, builder);
         break;
       case PS_SubNeg:
         stateSubNeg(state.local, ch, i, builder);
         break;
       case PS_SuboptNum:
         stateSuboptNum(state.local, ch, i, builder);
         break;
       case PS_Subopt:
         stateSubopt(state.local, ch);
         break;
       case PS_SuboptEscape:
         stateSuboptEscape(state.local, ch, i, builder);
         break;

       default:
         // This should never happen.
         assert(false);
         // Try to do something reasonable if it does.
         reset(builder);
         break;
      }
   }
   curpos_ += len;
}

/**
 * This function is called when the parent, for various reasons (like getting to
 * the end of the currently processed chunk) wants to wrap up processing and
 * move as much data into the 'parsed' state as possible.
 *
 * This is needed because data block telnet sections can be of arbitrary length
 * and have no 'end' delimeter as such.  They need to be processed as they come,
 * not in big blobs of 'all the data between telnet commands'.
 *
 * \param builder The builder to stuff any pending data chunks into.
 */
void TelnetParser::endOfChunk(TelnetChunkBuilder &builder)
{
   switch (state_)
   {
    case PS_Normal:
      regionend_ = curpos_;
      builder.addDataBlock(regionbegin_, regionend_);
      regionbegin_ = regionend_;
      break;
    case PS_Escape:
      // We've seen <IAC>
      assert(curpos_ >= 1);
      regionend_ = curpos_ - 1;
      assert(regionend_ >= regionbegin_);
      if (regionend_ > regionbegin_)
      {
         builder.addDataBlock(regionbegin_, regionend_);
      }
      regionbegin_ = regionend_;
      break;
    case PS_SuboptNum:
    case PS_SubNeg:
      // Seen <IAC> { <WILL>, <WONT>, <DO>, <DONT> }
      // or seen <IAC> <SE>
      assert(curpos_ >= 2);
      regionend_ = curpos_ - 2;
      assert(regionend_ >= regionbegin_);
      if (regionend_ > regionbegin_)
      {
         builder.addDataBlock(regionbegin_, regionend_);
      }
      regionbegin_ = regionend_;
   }
}

void TelnetParser::reset(TelnetChunkBuilder &builder)
{
   if ((state_ == PS_Subopt) || (state_ == PS_SuboptEscape))
   {
      // Abort/finish off any suboptions currently being parsed.
      regionend_ = curpos_;
      cooked_->resize(cooked_used_);
      {
         StrChunkPtrT<BufferChunk> tmp(cooked_);
         builder.addSuboption(subopt_type_, regionbegin_, regionend_, tmp);
      }
      cooked_ = 0;
      regionbegin_ = curpos_;
   }
   state_ = PS_Normal;
}
