/* $Header$ */

#ifdef __GNUG__
#pragma implementation "EchoModule.h"
#endif

// $Log$
// Revision 1.1  1995/07/22 04:46:49  hopper
// Initial revision
//
// Revision 0.9  1995/04/14  16:44:14  hopper
// Changed things for integration into the rest of my libraries.
//
// Revision 0.8  1995/04/05  01:48:45  hopper
// Changed things for integration into the rest of my libraries.
//
// Revision 0.7  1994/06/16  03:43:55  hopper
// Killed a virtual keyword where it shouldn't have been.
//
// Revision 0.6  1994/06/16  02:48:48  hopper
// Added #pragma implementation thing.
//
// Revision 0.5  1994/06/12  04:41:37  hopper
// 	Made major changes so EchoModule would work.
//
// Revision 0.4  1994/06/10  14:15:55  hopper
// Fixed include's to work better with OS2.
//
// Revision 0.3  1994/06/08  17:30:57  hopper
// Moved EchoModule::DeletePlug here.
//
// Revision 0.2  1994/06/08  17:16:36  hopper
// Fixed a syntax error/typo.
//
// Revision 0.1  1994/06/08  17:09:14  hopper
// Genesis!
//

// $Revision$


#ifndef OS2
#   include "StrMod/EchoModule.h"

#   ifndef _STR_StrChunk_H_
#      include "StrMod/StrChunk.h"
#   endif
#else
#   include "echomod.h"

#   ifndef _STR_StrChunk_H_
#      include "strchnk.h"
#   endif
#endif

#include <iostream.h>

const STR_ClassIdent EchoModule::identifier(3UL);
const STR_ClassIdent EchoPlug::identifier(4UL);

bool EchoModule::DeletePlug(StrPlug *plug)
{
   if (OwnsPlug(plug)) {
      plug->UnPlug();
      plugcreated = 0;
      if (buffedecho) {
	 cerr << "An EchoPlug was destroyed before some buffered output "
	    "was written. This is\nwhat should've been witten:\n";
	 buffedecho->PutIntoFd(2);
	 delete buffedecho;
	 buffedecho = 0;
      }
      return(1);
   } else
      return(0);
}

EchoModule::~EchoModule()
{
   DeletePlug(eplug);
   delete eplug;
}

StrChunk *EchoPlug::InternalRead()
{
   EchoModule *parent = ModuleFrom();

   if (CanRead()) {
      StrChunk *temp = parent->buffedecho;

      parent->buffedecho = 0;
      if (!(parent->wrtngto)) {
	 parent->rdngfrm = 1;
	 DoWriteableNotify();
	 parent->rdngfrm = 0;
      }
//      cerr << "Returned " << temp->Length() << " bytes from InternalRead()\n";
      return(temp);
   } else {
//      cerr << "Returned " << "NULL" << " bytes from InternalRead()\n";
      return(0);
   }
}

void EchoPlug::ReadableNotify()
{
   EchoModule *parent = ModuleFrom();

   if (!(parent->plugcreated))
      return;

   StrPlug *oplug = PluggedInto();

   while (oplug && CanWrite() && oplug->CanRead()) {
      StrChunk *curchnk;

      curchnk = oplug->Read();
      Write(curchnk);
      oplug = PluggedInto();
   }
}

void EchoPlug::WriteableNotify()
{
   EchoModule *parent = ModuleFrom();

   if (!(parent->plugcreated))
      return;

   StrPlug *oplug = PluggedInto();

   while (oplug && CanRead() && oplug->CanWrite()) {
      StrChunk *curchnk;

      curchnk = Read();
//      cerr << "oplug->Write(" << curchnk->Length() << " bytes)\n";
      oplug->Write(curchnk);
      oplug = PluggedInto();
   }
}

bool EchoPlug::Write(StrChunk *chnk)
{
   EchoModule *parent = ModuleFrom();

   if (!(parent->plugcreated) || parent->wrtngto)
      return(0);

   if (CanWrite()) {
      parent->buffedecho = chnk;
      parent->wrtngto = 1;
      if (parent->buffedecho != 0 && !(parent->rdngfrm))
	 DoReadableNotify();
      parent->wrtngto = 0;
      return(1);
   } else {
      StrChunk *temp = parent->buffedecho;

      parent->buffedecho = chnk;
      parent->wrtngto = 1;
      if (PluggedInto()->Write(temp)) {
	 if (parent->buffedecho != 0 && !(parent->rdngfrm))
	    DoReadableNotify();
	 parent->wrtngto = 0;
	 return(1);
      } else {
	 parent->buffedecho = temp;
	 parent->wrtngto = 0;
	 return(0);
      }
   }
}
