#ifndef _LCORE_Debugable_H_  // -*-c++-*-


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

// For a log, see ../ChangeLog

#include <iosfwd>

#define _LCORE_Debugable_H_

namespace strmod {
namespace lcore {

/** \class Debugable Debugable.h LCore/Debugable.h
 * Interface class for things that have certain common debugging functions.
 * You'll probable need to use a dynamic_cast to use this interface in a
 * generic way in most cases.
 */
class Debugable {
 public:
   virtual ~Debugable() = 0;

   /** \brief This should return false if the object is in a state that should
    *  be impossible.
    *
    * Your class invariant should be as strict as possible.  The class
    * invariant is used to detect when the class goes into a bad state, so all
    * possible bad states should be caught by the invariant.  */
   virtual bool invariant() const = 0;

   /** This should print out as much of the internal state of a class as would
    * be needed to debug it properly.
    */
   virtual void printState(::std::ostream &os) const = 0;
};

//--

// It's pure virtual, but it will be called directly by destructors in child
// classes.
inline Debugable::~Debugable() = default;

} // namespace lcore
} // namespace strmod

#endif
