#ifndef _STR_NewlineChopper_H_  // -*-c++-*-

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
//
// Revision 1.1  1996/09/02 23:18:48  hopper
// Added NewlineChopper class so users would have a simple class that
// would break up and recombine streams using '\n' as a separator.
//

#include <StrMod/CharChopper.h>

#define _STR_NewlineChopper_H_

class NewlineChopper : public CharChopper {
 public:
   typedef CharChopper parentclass;
   static const STR_ClassIdent identifier;

   NewlineChopper() : CharChopper('\n')                { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   NewlineChopper(const NewlineChopper &b);
};

//-----------------------------inline functions--------------------------------

inline int NewlineChopper::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || parentclass::AreYouA(cid));
}

#endif
