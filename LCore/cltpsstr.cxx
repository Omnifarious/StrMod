#include <iostream.h>
#include "LCore/ClassTypes.h"

 /* UNIX:@@:ClassTypesSTR.cc:@@: */
/* MSDOS:@@:CLTPSSTR.CPP:@@: */

void ClassIdent::PrintOn(ostream &os) const
{
   os << "ClassIdent(Programmer #" << GetProgrammer().GetPrNum();
   os << ", Class #" << GetClass().GetClNum() << ')';
}
