#ifndef _LCORE_ScopedPtr_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

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

/* $Header$ */

// For a log, see ../ChangeLog
// 
// Revision 1.2  1998/05/01 11:59:34  hopper
// Changed bool to bool_val or bool_cst as appropriate so it will be easier to
// port to platforms that don't support bool.
//
// Revision 1.1  1996/02/26 03:52:22  hopper
// This will be part of the standard C++ library someday, but isn't in
// libg++ 2.7.1
//

#include <bool.h>

#define _LCORE_ScopedPtr_H_

namespace strmod {
namespace lcore {

template<class T> class ScopedPtr {
 public:
   ScopedPtr(T *ptr) : myptr(ptr)                       { }
   inline ~ScopedPtr();

   T *operator ->()                                     { return(myptr); }
   const T *operator ->() const                         { return(myptr); }
   T &operator *()                                      { return(*myptr); }
   const T &operator *() const                          { return(*myptr); }

   T *operator T *()                                    { return(myptr); }
   const T *operator const T *() const                  { return(myptr); }
   bool_cst operator bool_cst() const                   { return(myptr != 0); }

   void Forget()                                        { myptr = 0; }

 private:
   T *myptr;
};

//-----------------------------inline functions--------------------------------

template<class T> ScopedPtr<T>::~ScopedPtr()
{
   if (myptr)
      delete myptr;
}

} // namespace lcore
} // namespace strmod

#endif
