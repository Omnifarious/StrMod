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

/**
 * \param ch The character to be stuffed into the state machine.
 * \param builder The TelnetChunkBuilder to use to build the parsed objects.
 *
 * Run the internal state machine on one character.  The internal state
 * machine will call the appropriate TelnetChunkBuilder methods.
 *
 * This allows the internal state machine to only pay attention to the
 * character sequence and be largely divorced from how the characters are
 * actually sliced and diced.
 */
void TelnetParser::processChar(U1Byte ch, TelnetChunkBuilder &builder)
{
   assert((state_ == PS_Normal) || (state_ == PS_Escape)
	  || (state_ == PS_SubNeg) || (state_ == PS_SuboptNum)
	  || (state_ == PS_Subopt) || (state_ == PS_SuboptEscape));

   switch (state_)
   {
    case PS_Normal:
      {
	 if (ch != TelnetChars::IAC)
	 {
            builder.addCharacter(ch);
	 }
	 else
	 {
	    state_ = PS_Escape;
	 }
      }
      break;

    case PS_Escape:
      {
         TelnetChars::Commands cmd;

         if (ch == TelnetChars::IAC)
         {
            state_ = PS_Normal;
            builder.addCharacter(ch);
         }
	 if (ch == TelnetChars::SB)
	 {
	    state_ = PS_SuboptNum;
	 }
	 else if (TelnetChars::convertCharToCommand(ch, cmd))
	 {
	    state_ = PS_Normal;
            builder.addCharCommand(cmd);
	 }
	 else if (TelnetChars::convertCharToOptionNegotiation(ch, negtype_))
	 {
	    state_ = PS_SubNeg;
	 }
	 else  // A silly escape came in.
	 {
	    state_ = PS_Normal;
	    // So do SOMETHING about it besides crash.
            builder.addIACAndCharacter(ch);
	 }
      }
      break;

    case PS_SubNeg:
      {
	 state_ = PS_Normal;
         builder.addNegotiationCommand(negtype_, ch);
      }
      break;

    case PS_SuboptNum:
      {
	 if (ch == 255) // We don't handle the extended suboption.
	 {
	    // This shouldn't happen.  Telnet engines using this parser should
	    // always say 'WONT' and 'DONT' to the extended suboption option.
	    // Some sort of reset anyway.
	    state_ = PS_Normal;
            // Don't do anything with the builder, act as if we never saw what
            // we saw.
	 }
	 else
	 {
            builder.initSuboption(ch);
	 }
      }
      break;

    case PS_Subopt:
      {
	 if (ch == TelnetChars::IAC)
	 {
	    state_ = PS_SuboptEscape;
	 }
         else
         {
            builder.addCharacter(ch);
         }
      }

    case PS_SuboptEscape:
      {
	 if (ch == TelnetChars::SE)
	 {
            state_ = PS_Normal;
            builder.finishSuboption();
         }
	 else
	 {
	    state_ = PS_Subopt;
            if (ch == TelnetChars::IAC)
            {
               builder.addCharacter(ch);
            }
            else
            {
               builder.addIACAndCharacter(ch);
            }
	 }
      }
      break;

    default:
      // This should never happen.
      assert(false);
      // Try to do something reasonable if it does.
      reset(builder);
      break;
   }
   // There is no return here so the compiler can catch if I've failed to
   // return in any of the cases.  One could say that I should assign a return
   // value to the variable, and return at the end, but that, in actuality, is
   // less safe because the error of forgetting to assign to the variable can
   // only be caught at run time.
}

void TelnetParser::reset(TelnetChunkBuilder &builder)
{
   state_ = PS_Normal;
   if (builder.isInSuboption())
   {
      builder.finishSuboption();
   }
}
