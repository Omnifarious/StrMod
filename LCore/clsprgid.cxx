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

#include "LCore/Object.h"

#ifndef _LCORE_ClassTypes_H_
#  include "LCore/ClassTypes.h"
#endif

#ifndef _LCORE_Programmers_H_
#  include "LCore/Programmers.h"
#endif

#ifndef _LCORE_HopClTypes_H_
#  include "LCore/HopClTypes.h"
#endif

 /* UNIX:@@:ClassProgID.cc:@@: */
/* MSDOS:@@:CLSPRGID.CPP:@@: */

namespace strmod {

/** Contains the basic underlying classes for the whole system
 *
 * This is the namespace for classes in the ClassIdent, ReferenceCounting, and
 * Protocol, LCoreError  subsystems.  Here are a list of the important top-level classes:
 *    - ClassIdent A globally (we mean globally) unique identifier for a class (deprecated).
 *    - Protocol A base class for classes that have a ClassIdent (deprecated)
 *    - Debugable A base class for classes that have a class invariant and can print useful state information.
 *    - ReferenceCounting A base class for objects with embedded reference counts. RefCountPtr is the smart pointer that works with ReferenceCounting
 *    - simple_bitset A simplified version of STLs ::std::bitset class.
 *    - enum_set A set of values of an enum (or enumerated) type.
 *    - LCoreError A nice class to use for throwing exceptions.
 */
namespace lcore {

const ProgrammerNum EricMHopper_0(0UL);
const ProgrammerNum WinterFire_0(2UL);
const ProgrammerNum StPaulSoftware_0(3UL);

const ClassIdent        Object::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       0UL));
const ClassIdent ProgrammerNum::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       1UL));
const ClassIdent      ClassNum::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       2UL));
const ClassIdent    ClassIdent::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       3UL));
const LCore_ClassIdent      EH_ClassNum::identifier(4UL);
const LCore_ClassIdent LCore_ClassIdent::identifier(5UL);

/*  This is a list of class #'s. Individual constants will be defined in
    seperate files to reduce the amount that is mandatorily linked into your
    program. The numbers here are here so there is one place to look to find
    and unused class #

  0UL	Object
  1UL	ProgrammerNum
  2UL	ClassNum
  3UL	ClassIdent
  4UL	EH_ClassNum
  5UL	LCore_ClassIdent
  6UL	EH_ClassIdent
  7UL	Protocol
  8UL	ReferenceCounting
  9UL	RefCountPtr
 10UL	Debugable

** End of class #'s */

} // namespace lcore
} // namespace strmod
