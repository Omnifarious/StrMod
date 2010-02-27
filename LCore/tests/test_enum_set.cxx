/*
 * Copyright 2001-2010 Eric M. Hopper <hopper@omnifarious.org>
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

#include <LCore/enum_set.h>

enum Barney { One = 2, Two, Three };

typedef enum_set<Barney, One, Three> BarneySet;

std::ostream &operator <<(std::ostream &os, const BarneySet &set)
{
   bool output = false;
   os << "(";
   if (set[One])
   {
      os << (output ? " | One" : "One");
      output = true;
   }
   if (set[Two])
   {
      os << (output ? " | Two" : "Two");
      output = true;
   }
   if (set[Three])
   {
      os << (output ? " | Three" : "Three");
      output = true;
   }
   os << (output ? ")" : "<nil>)");
   return os;
}

main()
{
   using std::cout;
   BarneySet joe;
   cout << joe << '\n';
   joe.set(One);
   cout << joe << '\n';
   joe.flip(One).flip(Three);
   cout << joe << '\n';
   joe.set(Two);
   cout << joe << '\n';
}
