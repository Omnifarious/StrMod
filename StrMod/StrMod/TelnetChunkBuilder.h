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
#include <cstddef>

#define _STR_TelnetChunkBuilder_H_

class BufferChunk;
template <class T> class StrChunkPtrT;

/** \class TelnetChunkBuilder TelnetChunkBuilder.h StrMod/TelnetChunkBuilder.h
 * Interface class for things that build telnet protocol data streams.
 *
 * This is an abstract interface used for building up telnet protocol elements
 * block by block.  The TelnetParser uses it, and the TelnetChunker implements
 * it to build up TelnetChunker::TelnetChunk objects.
 */
class TelnetChunkBuilder : virtual public Protocol {
 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   /** Add a data block to the stream.
    * \param regionbegin Where the region begins in some data buffer the child
    * class should know about (STL style).
    * \param regionend Where the region begins in some data buffer the child
    * class should know about (STL style).
    */
   virtual void addDataBlock(size_t regionbegin, size_t regionend) = 0;
   //! Add a single character telnet command to the stream.
   virtual void addCharCommand(TelnetChars::Commands command) = 0;
   /** Add a negotiation command to the stream.
    * Negotiations are where two sides of a telnet conversation agree to turn an
    * option off or on.  Sometimes turning an option on merely means being
    * willing to communicate more about it through suboption negotiation.
    */
   virtual void addNegotiationCommand(TelnetChars::OptionNegotiations negtype,
                                      U1Byte opt_type) = 0;
   /** Add a suboption to the data stream.
    * This function will cause a suboption of the specified type to be added to
    * the stream.
    * \param opt_type The telnet option this suboption is for.
    * \param regionbegin Where the raw suboption data region begins in some data
    * buffer the child class should know about (STL style).
    * \param regionend Where the raw suboption data region ends in some data
    * buffer the child class should know about (STL style).
    * \param cooked The 'cooked' data with all the escapes processed.
    */
   virtual void addSuboption(U1Byte opt_type,
                             size_t regionbegin, size_t regionend,
                             StrChunkPtrT<BufferChunk> &cooked) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline int TelnetChunkBuilder::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
