#ifndef _STR_TelnetChunker_H_  // -*-c++-*-

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

#define _STR_TelnetChunker_H_

/** \class TelnetChunker TelnetChunker.h StrMod/TelnetChunker.h
 * This class parses out a stream of data into seperate telnet protocol
 * elements.
 *
 * The protocol elements are either raw data, or a StrChunk derived from
 * TelnetParser::TelnetData.  For more information on what the protocol
 * elements mean, search for the relevant IETF RFCs, or visit
 * http://www.omnifarious.org/~hopper/technical/telnet-rfc.html
 */
class TelnetChunker : public StreamProcessor {
 public:
   class TelnetData;
   class SingleChar;
   class Suboption;
   class OptionNegotiation;

   static const STR_ClassIdent identifier;

   /** Construct a TelnetChunker.
    * Not much to tell one when you make it.  The state diagram has a quite
    * well defined starting point.
    */
   TelnetChunker();
   /** Destruct a TelnetChunker.
    * No pointers mean no explicit memory management here.
    */
   virtual ~TelnetChunker();

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   class Builder;

   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void processIncoming();

 private:
   class DataFunctor;
   class Internals;
   Internals &data_;
};

//-----------------------------inline functions--------------------------------

inline int TelnetChunker::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

#endif
