#include "LCore/Object.h"

 /* UNIX:@@:ObjectSTD.cc:@@: */
/* MSDOS:@@:OBJECTSTD.CPP:@@: */

int Object::DoEqual(const Object &b) const
{
   if (AreYouA(b.GetIdent()))
      return(b.IsEqual(*this));
   else if (b.AreYouA(GetIdent()))
      return(IsEqual(b));
   else
      return(0);
}
