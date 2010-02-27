#ifndef _STR_NewlineChopper_H_  // -*-c++-*-

/*
 * Copyright 1991-2002 Eric M. Hopper <hopper@omnifarious.org>
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
//
// Revision 1.1  1996/09/02 23:18:48  hopper
// Added NewlineChopper class so users would have a simple class that
// would break up and recombine streams using '\n' as a separator.
//

#include <StrMod/CharChopper.h>

#define _STR_NewlineChopper_H_

namespace strmod {
namespace strmod {

/** \class NewlineChopper NewlineChopper.h StrMod/NewlineChopper.h
 * \brief Just a CharChopper that's hard wired to '\\n' as the character.
 *
 * This is such a common case, I thought it deserved its own class.
 */
class NewlineChopper : public CharChopper
{
 public:
   typedef CharChopper parentclass;

   //! Just pass a newline character to parent consructor.
   NewlineChopper() : CharChopper('\n')                { }
   // Derived class destructor doesn't do anything base class one doesn't do.

 private:
   NewlineChopper(const NewlineChopper &b);
};

};  // namespace strmod
};  // namespace strmod

#endif
