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
