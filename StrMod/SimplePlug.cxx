/* $Header$ */

// $Log$
// Revision 1.1  1996/08/24 12:55:09  hopper
// New source module from SimplePlug class, and SimplePlugT<T> template.
//

#ifdef __GNUG__
#  pragma implementation "SimplePlug.h"
#endif

#include <StrMod/SimplePlug.h>

const STR_ClassIdent SimplePlug::identifier(24UL);

void SimplePlug::ReadableNotify()
{
   StrPlug *mate = PluggedInto();

   writeflag_ = true;
   while (mate && mate->CanRead() && i_CanWrite()) {
      i_Write(mate->Read());
      mate = PluggedInto();
   }
   writeflag_ = false;
}

void SimplePlug::WriteableNotify()
{
   StrPlug *mate = PluggedInto();

   readflag_ = true;
   while (mate && mate->CanWrite() && i_CanRead()) {
      mate->Write(i_InternalRead());
      mate = PluggedInto();
   }
   readflag_ = false;
}
