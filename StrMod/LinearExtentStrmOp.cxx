/* $Header$ */

// $Log$
// Revision 1.1  1996/06/29 06:53:20  hopper
// New file for implementation of stream operator for class LinearExtent.
//

#include <StrMod/LinearExtent.h>
#include <iostream.h>

ostream &operator <<(ostream &os, const LinearExtent &ext)
{
#ifdef __GNUG__
   unsigned long long end;
#else
   unsigned long end;
#endif

   end = ext.Offset();
   end += ext.Length();
   os << "[" << ext.Offset() << "--(" << ext.Length() << ")-->" << end << ")";
}
