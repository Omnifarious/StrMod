#ifndef _STR_TelnetParser_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#define _STR_TelnetParser_H_

#include <StrMod/TelnetChars.h>
#include <StrMod/STR_ClassIdent.h>
#include <LCore/Protocol.h>
#include <LCore/HopClTypes.h>
#include <cstddef>

class TelnetChunkBuilder;
template <int n> class PreAllocBuffer;

/** \class TelnetParser TelnetParser.h StrMod/TelnetParser.h
 * Class for parsing out a stream of characters into telnet protocol
 * elements using the TelnetChunkBuilder class.
 */
class TelnetParser : virtual public Protocol {
 public:
   static const STR_ClassIdent identifier;

   //! Construct a parser in the 'start' state.
   TelnetParser();
   //! Destroy a parser.
   virtual ~TelnetParser();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //! Process a buffer, calling the builder, and advancing the state.
   void processData(const void *data, size_t len,
                            TelnetChunkBuilder &builder);

   //! Stuff any pending data chunks into the builder.
   void endOfChunk(TelnetChunkBuilder &builder);

   /** \brief Return number of bytes to the beginning of the current region that
    * TelnetParser is not finished parsing yet.
   */
   size_t getRegionBegin() const                        { return regionbegin_; }

   /** \brief Move region as if <code>bytes</code> bytes had been chopped off
    * the front.
    */
   inline void dropBytes(size_t bytes);

   //! Reset the parser back to the 'start' state.
   void reset(TelnetChunkBuilder &builder);

 protected:
   /** Describes the current state of the parser.
    *
    * This state is completely determined by the characters fed into
    * processChar().
    */
   enum ParserState {
      PS_Normal,  //!< Starting state.  Normal data.
      PS_Escape,  //!< Saw an IAC character in normal data
      PS_SubNeg,  //!< Saw an IAC {WILL,WONT,DO,DONT}, expecting option number
      PS_SuboptNum, //!< Saw an IAC SE and expecting an option number
      PS_Subopt,  //!< Between IAC SE <num> and IAC SE
      PS_SuboptEscape  //!< Saw an IAC while in PS_Subopt
   };

   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

 private:
   TelnetChars::OptionNegotiations negtype_;
   U1Byte subopt_type_;
   size_t curpos_;
   ParserState state_;
   size_t regionbegin_;
   size_t regionend_;
   U1Byte *cookedbuf_;
   size_t cooked_total_;
   size_t cooked_used_;
   PreAllocBuffer<48> *cooked_;

   inline void stateNormal(ParserState &state, const U1Byte ch);
   inline void stateEscape(ParserState &state, const U1Byte ch, size_t i,
                           TelnetChunkBuilder &builder);
   inline void stateSubNeg(ParserState &state, const U1Byte ch, size_t i,
                           TelnetChunkBuilder &builder);
   inline void stateSuboptNum(ParserState &state, const U1Byte ch, size_t i,
                              TelnetChunkBuilder &builder);
   inline void stateSubopt(ParserState &state, U1Byte ch);
   inline void stateSuboptEscape(ParserState &state, U1Byte ch, size_t i,
                                 TelnetChunkBuilder &builder);
};

//-----------------------------inline functions--------------------------------

inline int TelnetParser::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline void TelnetParser::dropBytes(size_t bytes)
{
   if (bytes <= regionbegin_)
   {
      regionbegin_ -= bytes;
      curpos_ -= bytes;
   }
}

#endif
