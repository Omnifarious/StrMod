/* $Header$ */

#ifdef __GNUG__
#pragma implementation "EchoModule.h"
#endif

// $Log$
// Revision 1.3  1996/08/31 15:50:13  hopper
// Changed to use StrChunkPtr::operator bool a little more explicity.  Used
// to compare pointer values to 0.
//
// Revision 1.2  1996/07/06 01:23:22  hopper
// Changed to use new StrChunkPtr interface, and new parent module stuff.
// Streamlined and buffed up other stuff.
//
// Revision 1.1.1.1  1995/07/22 04:46:49  hopper
// Imported sources
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


#include "StrMod/EchoModule.h"

#ifndef _STR_StrChunk_H_
#  include "StrMod/StrChunk.h"
#endif

#ifndef _STR_StrChunk_H_
#  include "strchnk.h"
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
	    "was written.\n";
	 buffedecho.ReleasePtr();
      }
      return(true);
   } else
      return(false);
}

EchoModule::~EchoModule()
{
   DeletePlug(eplug);
   delete eplug;
}

#ifdef __GNUG__
const StrChunkPtr EchoPlug::InternalRead() return temp;
#else
const StrChunkPtr EchoPlug::InternalRead()
#endif
{
   EchoModule *parent = ModuleFrom();

   if (CanRead()) {
      parent->rdngfrm = 1;
#ifdef __GNUG__
      temp = parent->buffedecho;
#else
      StrChunkPtr temp = parent->buffedecho;
#endif

      parent->buffedecho.ReleasePtr();
      if (!(parent->wrtngto)) {
	 DoWriteableNotify();
      }
//     cerr << "Returned " << temp->Length() << " bytes from InternalRead()\n";
      parent->rdngfrm = 0;
      return(temp);
   } else {
//      cerr << "Returned " << "NULL" << " bytes from InternalRead()\n";
#ifdef __GNUG__
      temp = 0;
      return(temp);
#else
      return(0);
#endif
   }
}

bool EchoPlug::Write(const StrChunkPtr &chnk)
{
   EchoModule *parent = ModuleFrom();

   if (!(parent->plugcreated) || parent->wrtngto)
      return(false);

   if (CanWrite()) {
//      cerr << "EchoPlug::Write - 1\n";
      parent->wrtngto = 1;
      parent->buffedecho = chnk;
      if (parent->buffedecho && !(parent->rdngfrm))
	 DoReadableNotify();
      parent->wrtngto = 0;
      return(true);
   } else {
//      cerr << "EchoPlug::Write - 2\n";
      parent->wrtngto = 1;
      StrChunkPtr temp = parent->buffedecho;

      parent->buffedecho = chnk;
      if (PluggedInto()->Write(temp)) {
	 if (parent->buffedecho && !(parent->rdngfrm))
	    DoReadableNotify();
	 parent->wrtngto = 0;
	 return(true);
      } else {
	 parent->buffedecho = temp;
	 parent->wrtngto = 0;
	 return(false);
      }
   }
}
