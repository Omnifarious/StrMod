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

#include <string>
#include <EHnet++/InetAddress.h>
#include <iostream>

using std::cout;

int main(int argc, char *argv[])
{
   if (argc <= 1) {
      InetAddress iaddr(0);

      cout << "iaddr == \"" << iaddr.AsString() << "\"\n";
   } else {
      string name = argv[1];
      InetAddress iaddr(name, 0);

      cout << "iaddr == \"" << iaddr.AsString() << "\"\n";
   }
   return(0);
}
