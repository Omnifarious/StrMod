#ifndef _STR_LocalCopy_H_  // -*-c++-*-

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

#define _STR_LocalCopy_H_

/** \class LocalCopy LocalCopy.h StrMod/LocalCopy.h
 * \brief Makes a local copy of a variable the compiler doesn't have enough
 * information to do CSE on.
 *
 * This will make a local copy of a member variable and store the value of
 * that copy back into the member variable when the local copy is destroyed.
 * This is useful for a member variable you know will only be altered by the
 * function across an entire call, despite other functions being called.  When
 * other functions are called, the compiler's CSE throws out member variables,
 * as those variables may concievably be altered by the function call.
 *
 */
template <class T>
class LocalCopy {
 public:
   /** Constructs a local copy of a member or global variable.
    * @param classvar A reference to the variable to make a copy of.
    */
   LocalCopy(T &classvar) : local(classvar), save_(classvar)  { }
   /** \brief Stores the local copy back to original member or global variable
    * referenced by classvar in the constructor.
    */
   ~LocalCopy()                                               { save_ = local; }

   T local;  //!< Yes, this is supposed to be public.

   //! Trying to make this stand in for the variable as much as possible.
   operator T &()                                             { return local; }
   //! Trying to make this stand in for the variable as much as possible.
   operator const T &() const                                 { return local; }
   //! Trying to make this stand in for the variable as much as possible.
   inline const T &operator =(const T &b);

 private:
   T &save_;
};

//-----------------------------inline functions--------------------------------

template <class T>
const T &LocalCopy<T>::operator =(const T &b)
{
   return local = b;
}

#endif
