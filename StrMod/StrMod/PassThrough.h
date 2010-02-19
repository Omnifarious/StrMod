#ifndef _STR_PassThrough_H_  // -*-c++-*-

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

// For a log, see ../Changelog
//
// Revision 1.1  1996/09/02 23:26:48  hopper
// Added PassThrough class to use when you needed a StreamProcessor that
// did nothing except buffer one Chunk worth of data.
//

#include <StrMod/StreamProcessor.h>
#include <cassert>

#define _STR_PassThrough_H_

namespace strmod {
namespace strmod {

class PassThrough : public StreamProcessor {
 public:
   static const STR_ClassIdent identifier;

   PassThrough()                                       { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual void processIncoming();

 private:
   PassThrough(const PassThrough &b);
};

//-----------------------------inline functions--------------------------------

inline int PassThrough::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

inline void PassThrough::processIncoming()
{
   assert(!outgoing_);
   outgoing_ = incoming_;
   outgoing_ready_ = true;
   incoming_.ReleasePtr();
}

};  // namespace strmod
};  // namespace strmod

#endif
