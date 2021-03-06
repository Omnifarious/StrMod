/* $Header$ */
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

// For a log, see ./ChangeLog

#ifdef __GNUG__
#  pragma implementation "LCoreError.h"
#endif

#include "LCore/LCoreError.h"
#include <iostream>

namespace strmod {
namespace lcore {

::std::ostream &operator <<(::std::ostream &os, const LCoreError &err)
{
   bool out = false;
   if (err.getSourceFile())
   {
      os << "In " << err.getSourceFile();
      out = true;
   }
   if (err.getLine() != 0)
   {
      os << (out ? ", at line " : "At line ") << err.getLine();
      out = true;
   }
   if (err.getFunc())
   {
      os << (out ? ", in function '" : "In function '") << err.getFunc();
   }
   os << (out ? ": " : "Error: ")
      << (err.getDesc() ? err.getDesc() : "<no description>");
   return os;
}

} // namespace lcore
} // namespace strmod
