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

// See ../ChangeLog file for log

#include "UniEvent/SimpleDispatcher.h"
#include "testdisp.h"
#include <iostream.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <seed value>\n";
      return(1);
   } else {
      UNISimpleDispatcher disp;

      TestDispatcher(disp, atoi(argv[1]));
   }
   return(0);
}
