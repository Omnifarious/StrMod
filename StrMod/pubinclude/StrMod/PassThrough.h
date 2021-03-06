#ifndef _STR_PassThrough_H_  // -*-c++-*-

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

/** \class PassThrough PassThrough.h StrMod/PassThrough.h
 * This is a StreamProcessor that does nothing.
 *
 * Oftentimes, when using a ProcessorModule, you will want no processing to be
 * done in one direction or the other.  That's what PassThrough StreamProcessor
 * is for.
 */
class PassThrough : public StreamProcessor
{
 public:
   //! Doesn't do anything, so doesn't need much in its constructor.
   PassThrough()                                       { }
   // Derived class destructor doesn't do anything base class one doesn't do.

 protected:
   inline void processIncoming() override;

 private:
   PassThrough(const PassThrough &b);
};

//-----------------------------inline functions--------------------------------

inline void PassThrough::processIncoming()
{
   assert(!outgoing_);
   outgoing_ = incoming_;
   outgoing_ready_ = true;
   incoming_.reset();
}

}  // namespace strmod
}  // namespace strmod

#endif
