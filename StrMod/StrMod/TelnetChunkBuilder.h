#ifndef _STR_TelnetChunkBuilder_H_  // -*-c++-*-

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

#include <StrMod/STR_ClassIdent.h>
#include <StrMod/TelnetChars.h>
#include <LCore/Protocol.h>
#include <LCore/HopClTypes.h>
#include <stdexcept>

#define _STR_TelnetChunkBuilder_H_

/** \class TelnetChunkBuilder TelnetChunkBuilder.h StrMod/TelnetChunkBuilder.h
 * Interface class for things that build telnet protocol data streams.
 *
 * This is an abstract interface used for building up telnet protocol
 * elements character by character.  The TelnetParser uses it, and the
 * TelnetChunker implements it to build up TelnetChunk objects.
 */
class TelnetChunkBuilder : virtual public Protocol {
 public:
   /** \class bad_call_order TelnetChunkBuilder.h StrMod/TelnetChunkBuilder.h
    * Exception used to indicate functions being called in the wrong order.
    */
    class bad_call_order : public std::logic_error {
    public:
       //! Just calls the logic_error constructor.
       bad_call_order(const string &what) : logic_error(what)     { }
   };

   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   /** Add a data character to the stream.
    * If isInSuboption() returns true, this will add a character to the
    * suboption data.
    */
   virtual void addCharacter(U1Byte c) = 0;
   /** Add IAC (255) and a character to the data chunk.
    * If isInSuboption() returns true, this will add characters to the
    * suboption data.
    */
   virtual void addIACAndCharacter(U1Byte c) = 0;
   /** Add a single character telnet command to the stream.
    * Can't be called while isInSuboption() returns true.
    */
   virtual void addCharCommand(TelnetChars::Commands command)
      throw(bad_call_order) = 0;
   /** Add a negotiation command to the stream.
    * Can't be called while isInSuboption() returns true.
    *
    * Negotiations are where two sides of a telnet conversation agree to turn
    * an option off or on.  Sometimes turning an option on merely means being
    * willing to communicate more about it through suboption negotiation.
    */
   virtual void addNegotiationCommand(TelnetChars::OptionNegotiations negtype,
                                      U1Byte opt_type)
      throw(bad_call_order) = 0;

   /** Whether the builder thinks it's currently processing suboption daa.
    * If this function returns true, the meaning of addCharacter(), and
    * addIACAndCharacter() are changed slightly, and it becomes illegal to
    * call addCharCommand(), addNegotiationCommand(), and initSuboption().
    *
    * If this function returns false, it's illegal to call finishSuboption().
    */
   virtual bool isInSuboption() const = 0;

   /** \name Suboption bracketing functions
    *
    * While the WILL, WONT, DO, DONT protocol is good for negotiation whether
    * an option is off or on, sometimes there's more to communicate about an
    * option.  For example, you can use WILL, WONT, DO, DONT to decide if you
    * agree to communicate about terminal types, but you have to send over
    * what types of terminals you support somehow.  That's where suboption
    * negotiation is used.
    *
    * Since the data in a suboption is highly option specific, the telnet
    * protocol defines a general way to communicate that data so that you
    * don't have to have special parsing routines for each suboption to lift
    * the data from the stream.
    *
    * The initSuboption() and finishSuboption() functions are intended to
    * mirror the delimeters surrounding suboption data so the builder can
    * determines which data belongs as part of a suboption.
    * */
   //@{
   /** Begin suboption data for option number opt_type.
    * From the time this function is called until finishSuboption() is called,
    * isInSuboption() will return true.
    */
   virtual void initSuboption(U1Byte opt_type) throw(bad_call_order) = 0;
   /** End the current suboption.
    * This function will cause the suboption data to be added to the stream,
    * and isInSuboption() will return false again.
    */
   virtual void finishSuboption() throw(bad_call_order) = 0;
   //@}

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline int TelnetChunkBuilder::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
