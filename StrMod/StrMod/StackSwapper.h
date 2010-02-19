#ifndef _STR_StackSwapper_H_  // -*-c++-*-

/*
 * Copyright 2000 by Eric M. Hopper <hopper@omnifarious.mn.org>
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

// For a log, see ../ChangLog

#define _STR_StackSwapper_H_

namespace strmod {
namespace strmod {

/** \class StackSwapper StackSwapper.h StrMod/StackSwapper.h
 * A class to make a certain means of doing dynamic scoping in a lexically
 * scoped language exception safe.
 *
 * This could be thought of as maintaining a particular global or member
 * variable (<code>classvar</code>) as the top stack value in a recursive
 * function that maintained a stack of values on the execution stack.
 *
 * This establishes something akin to dynamic scope.
 *
 * Using the constructor and destructor to maintain these values causes this
 * manipulation to be exception safe.  It eliminates the need for a lot of
 * <code>try { } catch (...) { }</code> blocks to clean stuff up and make
 * things exception safe. */
template <class T> class StackSwapper {
 public:
   /** Saves the value in classvar, then sets classvar to newval.
    * @param classvar A reference to the variable who's valued is to be saved,
    * then restored on scope exit.
    *
    * @param newval The new value to be stored in classvar.
    */
   inline StackSwapper(T &classvar, const T &newval);
   /** \brief Restores the value of the variable referenced by classvar in the
    * constructor.
    */
   inline ~StackSwapper();

 private:
   T &classvar_;
   T oldval_;

   // Left private and undefined on purpose.
   void operator =(const StackSwapper<T> &b);
   StackSwapper(const StackSwapper &b);
};

//-----------------------------inline functions--------------------------------

template <class T>
inline StackSwapper<T>::StackSwapper(T &classvar, const T &newval)
     : classvar_(classvar), oldval_(classvar)
{
   classvar = newval;
}

template <class T>
inline StackSwapper<T>::~StackSwapper()
{
   classvar_ = oldval_;
}

};  // namespace strmod
};  // namespace strmod

#endif
