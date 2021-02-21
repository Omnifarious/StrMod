#ifndef _STR_TelnetChars_H_  // -*-c++-*-

/*
 * Copyright 2001-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For a log, see ../ChangLog

#include <LCore/GenTypes.h>

#define _STR_TelnetChars_H_

namespace strmod {
namespace strmod {

/** \class TelnetChars TelnetChars.h StrMod/TelnetChars.h
 * A class holding a bunch of telnet character constants.
 *
 * Perhaps this class should be a namespace instead.
 */
class TelnetChars {
 public:
   typedef lcore::U1Byte U1Byte;
   static const U1Byte  TEOF = 236U;  //!< End Of File
   static const U1Byte  SUSP = 237U;  //!< Suspend process
   static const U1Byte ABORT = 238U;  //!< Abort process
   static const U1Byte   EOR = 239U;  //!< End Of Record
   static const U1Byte    SE = 240U;  //!< Suboption End
   static const U1Byte   NOP = 241U;  //!< No Operation
   static const U1Byte    DM = 242U;  //!< Data mark, use with Synch option, requires out-of-band data support
   static const U1Byte   BRK = 243U;  //!< Break  (often a long string of 0 bits in RS232)
   static const U1Byte    IP = 244U;  //!< Interrupt Process
   static const U1Byte    AO = 245U;  //!< Abort output
   static const U1Byte   AYT = 246U;  //!< Are You There?  Remote side expected to reply with 'Yes'
   static const U1Byte    EC = 247U;  //!< Erease Character  (like backspace)
   static const U1Byte    EL = 248U;  //!< Erase Line
   static const U1Byte    GA = 249U;  //!< Go Ahead (for half duplex (the default) connections)
   static const U1Byte    SB = 250U;  //!< Suboption Begin
   static const U1Byte  WILL = 251U;  //!< Sender wants to enable option for itself
   static const U1Byte  WONT = 252U;  //!< Sender wants to disable option for itself
   static const U1Byte    DO = 253U;  //!< Sender wants receiver to enable option
   static const U1Byte  DONT = 254U;  //!< Sender wants receiver to disable option
   static const U1Byte   IAC = 255U;  //!< Interpret As Command

   /** An enum of all the telnet single character commands.
    * This is here so that functions that expect a telnet special character
    * representing a telnet single character command can say that's what they
    * want instead of generically expecting a character and being upset when
    * it doesn't qualify.
    *
    * These are defined in RFCs <A HREF="http://info.internet.isi.edu/in-notes/rfc/files/rfc854.txt">854</A>,
    * <A HREF="http://info.internet.isi.edu/in-notes/rfc/files/rfc885.txt">885</A>,
    * and <A HREF="http://info.internet.isi.edu/in-notes/rfc/files/rfc1184.txt">1184</A>
    */
   enum Commands {
      C_EOF = TEOF,	//!< See TEOF
      C_SUSP = SUSP,	//!< See SUSP
      C_ABORT = ABORT,	//!< See ABORT
      C_EOR = EOR,	//!< See EOR
      C_NOP = NOP,	//!< See NOP
      C_DM = DM,	//!< See DM
      C_BRK = BRK,	//!< See BRK
      C_IP = IP,	//!< See IP
      C_AO = AO,	//!< See AO
      C_AYT = AYT,	//!< See AYT
      C_EC = EC,	//!< See EC
      C_EL = EL,	//!< See EL
      C_GA = GA		//!< See GA
   };

   /** Convert a character to an enum value in the Commands enum, if possible.
    * \param ch The character to be converted.
    * \param cmd The character's value in the Commands enum if conversion possible, no modification if not.
    * \return true if conversion successful, false if not.
    */
   inline static bool convertCharToCommand(U1Byte ch, Commands &cmd);

   /** An enum of all the different types of option negotiation there are.
    * This is here so that functions that expect an option negotiation
    * character can say that's what they want instead of generically expecting
    * a character and being upset when it's the wrong one.
    *
    * The telnet protocol is designed as a peer-to-peer protocol, and so
    * either side can initiate an option negotiation to enable or disable an
    * option for either half (or direction) of the conversation.  A request to
    * disable must always be honored, and a refusal to enable must always be
    * honored.
    */
   enum OptionNegotiations {
      O_WILL = WILL, //!< Sender wants to enable option for itself
      O_WONT = WONT, //!< Sender wants to disable option for itself
      O_DO = DO,     //!< Sender wants receiver to enable option
      O_DONT = DONT  //!< Sender wants receiver to disable option
   };

   /** Convert a character to an enum value in the OptionNegotiations enum, if possible.
    * \param ch The character to be converted.
    * \param opt The character's value in the OptionNegotiations enum if conversion possible, no modification if not.
    * \return true if conversion successful, false if not.
    */
   inline static bool convertCharToOptionNegotiation(U1Byte ch,
                                                     OptionNegotiations &opt);
};

//-----------------------------inline functions--------------------------------

inline bool TelnetChars::convertCharToCommand(U1Byte ch,
                                              TelnetChars::Commands &cmd)
{
   if (((ch >= C_EOF) && (ch <= C_EOR)) ||
       ((ch >= C_NOP) && (ch <= C_GA)))
   {
      cmd = static_cast<Commands>(ch);
      return true;
   }
   return false;
}

inline bool
TelnetChars::convertCharToOptionNegotiation(
   U1Byte ch, TelnetChars::OptionNegotiations &opt
   )
{
   if ((ch >= WILL) && (ch <= DONT))
   {
      opt = static_cast<OptionNegotiations>(ch);
      return true;
   }
   return false;
}

}  // namespace strmod
}  // namespace strmod

#endif
