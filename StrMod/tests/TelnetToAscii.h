#ifndef _STRT_TelnetToAscii_H_  // -*-c++-*-

/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For a log, see ../Changelog

#include <StrMod/StreamProcessor.h>
#include <StrMod/StrChunkPtr.h>
#include <LCore/HopClTypes.h>
#include <string>

#define _STRT_TelnetToAscii_H_

class TelnetToAscii : public StreamProcessor {
 public:
   static const EH_ClassIdent identifier;

   explicit TelnetToAscii(bool stripdata = false);
   explicit TelnetToAscii(const string &name, bool stripdata = false);
   virtual ~TelnetToAscii() = default;

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void processIncoming();

 private:
   StrChunkPtr namechunk_;
   bool stripdata_;
};

//-----------------------------inline functions--------------------------------

int TelnetToAscii::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

#endif
