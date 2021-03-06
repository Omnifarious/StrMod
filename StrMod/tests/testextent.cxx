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

#include <StrMod/LinearExtent.h>
#include <iostream>

std::ostream &operator <<(std::ostream &os, LinearExtent &ext)
{
   os << '[' << ext.Offset() << '_' << ext.Length() << "_)";
}

LinearExtent TestExtent()
{
   LinearExtent joe(0, 30);
   LinearExtent george(joe.SubExtent(LinearExtent(10, 5)));

   cout << joe << ", " << george << '\n';
   george.LengthenRight(5);
   cout << "___george___\n";
   cout << george << '\n';
   george.LengthenLeft(5);
   cout << george << '\n';
   george.LengthenCenter(5);
   cout << george << '\n';
   joe.ShortenRight(5);
   cout << "___joe___\n";
   cout << joe << '\n';
   joe.ShortenLeft(5);
   cout << joe << '\n';
   joe.LengthenCenter(5);
   cout << joe << '\n';
   joe.ShortenCenter(5);
   cout << joe << '\n';
   joe.ShortenCenter(5);
   cout << joe << '\n';
   return(joe.SubExtent(george));
}

main()
{
   LinearExtent test = TestExtent();

   cout << "test == " << test << '\n';
}
