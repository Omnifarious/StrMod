#ifndef _STR_TelnetParser_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog

#include <StrMod/StreamProcessor.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/PreAllocBuffer.h>
#include <StrMod/DynamicBuffer.h>
#include <LCore/GenTypes.h>
#include <cstddef>

#define _STR_TelnetParser_H_

/** \class TelnetParser TelnetParser.h StrMod/TelnetParser.h
 * This class parses out a stream of data into seperate telnet protocol
 * elements.
 *
 * The protocol elements are either raw data, or a StrChunk derived from
 * TelnetParser::TelnetData.  For more information on what the protocol
 * elements mean, search for the relevant IETF RFCs, or visit
 * http://www.omnifarious.org/~hopper/technical/telnet-rfc.html
 */
class TelnetParser : public StreamProcessor {
 public:
   class TelnetData;
   class SingleChar;
   class Suboption;
   class OptionNegotiation;

   static const STR_ClassIdent identifier;
   static const U1Byte TN_IAC = 255U; //!< Byte meaning 'Interpret As Command'
   static const U1Byte TN_SB = 250U; //!< Suboption Begin
   static const U1Byte TN_SE = 240U; //!< Suboption End

   /** Construct a TelnetParser.
    * Not much to tell one when you make it.  The state diagram has a quite
    * well defined starting point.
    */
   TelnetParser();
   /** Destruct a TelnetParser.
    * No pointers mean no explicit memory management here.
    */
   virtual ~TelnetParser();

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   //! The actions that processChar can state need to be performed.
   enum Actions { PC_AddCurrentChar, //!< Add the character just procesed to the outgoing data buffer.
                  PC_AddIAC,  //!< Add TN_IAC to the outgoing data buffer.
                  PC_AddIACAndCurrentChar,  //!< Add TN_IAC, and the character just processed to the outgoing data buffer.
                  PC_AddSingleCharCommand,  //!< Add a TelnetParser::SingleChar StrChunk to the output stream.
                  PC_AddWILL,  //!< Add an appropriate TelnetParser::OptionNegotiation StrChunk to the output stream.
                  PC_AddWONT,  //!< Add an appropriate TelnetParser::OptionNegotiation StrChunk to the output stream.
                  PC_AddDO,  //!< Add an appropriate TelnetParser::OptionNegotiation StrChunk to the output stream.
                  PC_AddDONT,  //!< Add an appropriate TelnetParser::OptionNegotiation StrChunk to the output stream.
                  PC_StartSubopt,  //!< Start collecting suboption data.
                  PC_AddSuboptChar,  //!< Add current character to suboption data.
                  PC_AddSuboptIAC,  //!< Add TN_IAC to suboption data.
                  PC_AddSuboptIACAndCurrentChar,  //!< Add TN_IAC and current char to suboption data.
                  PC_EndSubopt,  //!< End suboption and add TelnetParser::Suboption to output stream.
                  PC_DoNothing  //!< Exactly what it says.
   };

   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void processIncoming();

   //! Run the internal state machine on one character.
   Actions processChar(U1Byte ch);

 private:
   enum ParseStates { PS_Normal,  //!< Start state, normal characters
                      PS_Escape,  //!< IAC has been encountered.
                      PS_SubNeg,  //!< Suboption support negotion WILL, WONT, DO, DONT
		      PS_SuboptNum, //!< Getting the suboption number for suboption negotiation
                      PS_Subopt, //!< Reading the suboption data
                      PS_SuboptEscape  //!< Encountered IAC in suboption data.
   };
   typedef PreAllocBuffer<32> SuboptBuffer;

   static const size_t subopt_size_limit_ = 65536U;

   size_t incoming_pos_;
   StrChunkPtr telnetcommand_;

   // Parser stuff.
   ParseStates state_;
   // For collecting data.
   StrChunkPtrT<DynamicBuffer> data_;
   size_t datasize_;
   // For collecting suboption data
   StrChunkPtrT<SuboptBuffer> cookedsubopt_;
   size_t cookedsize_;
   StrChunkPtrT<SuboptBuffer> rawsubopt_;
   size_t rawsize_;
   U1Byte negtype_;    //< Used to indicate WILL, WONT, DO, or DONT
   U1Byte suboptnum_;  //< Used when a suboption # is part of the state.

   inline static void addToChunk(StrChunkPtrT<SuboptBuffer> &chnk,
				 size_t &cursize, U1Byte ch);
};

//-----------------------------inline functions--------------------------------

inline int TelnetParser::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

#endif
