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

#include <iostream.h>
#include "LCore/Object.h"

 /* UNIX:@@:ObjectSTR.cc:@@: */
/* MSDOS:@@:OBJCTSTR.CPP:@@: */

void Object::PrintOn(ostream &os) const
{
   if (Object::identifier == GetIdent())
      os << "an Object (why someone created just an object is beyond me)";
   else {
      os << "something derived from Object that hasn't overridden its PrintOn methods\n";
      os << "class id is: " << *((const Object *)(&(GetIdent()))) << '\n';
   }
}
