/* $Header$ */

 // $Log$
 // Revision 1.4  1996/07/07 20:56:49  hopper
 // Added, then commented out some debugging.  I left it there because it
 // might be useful someday.
 //
 // Revision 1.3  1996/07/05 19:38:14  hopper
 // Added forgotten StrPlug::WriteableNotify implementation.
 //
 // Revision 1.2  1996/07/05 18:50:05  hopper
 // Changed to use new StrChunkPtr interface.  Also changed to implement
 // new parent module handling in StrPlug base class.
 //
 // Revision 1.1.1.1  1995/07/22 04:46:49  hopper
 // Imported sources
 //
 // -> Revision 0.12  1995/04/14  16:42:55  hopper
 // -> Combined versions 0.11 and 0.11.0.4
 // ->
 // -> Revision 0.11.0.4  1995/04/14  16:41:23  hopper
 // -> Added declarations for StreamModule identifiers.
 // ->
 // -> Revision 0.11.0.3  1995/04/05  01:48:34  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.11.0.2  1994/07/18  03:44:56  hopper
 // -> Added #pragma implementation thing so this would work better with gcc 2.6.0
 // ->
 // -> Revision 0.11.0.1  1994/05/16  06:12:19  hopper
 // -> No changes, but I needed a head for the inevitable WinterFire-OS/2
 // -> branch.
 // ->
 // -> Revision 0.11  1994/05/07  03:24:53  hopper
 // -> Changed header files stuff around to be aprroximately right with
 // -> new libraries, and new names & stuff.
 // ->
 // -> Revision 0.10  1992/04/14  20:47:39  hopper
 // -> Genesis!
 // ->

#ifndef NO_RcsID
static char _EH_StreamModule_CC_rcsID[] = "$Id$";
#endif

#ifdef __GNUG__
#  pragma implementation "StreamModule.h"
#endif

#include "StrMod/StreamModule.h"
// #include <iostream.h>

const STR_ClassIdent StreamModule::identifier(1UL);
const STR_ClassIdent StrPlug::identifier(2UL);

bool StrPlug::PlugInto(StrPlug *p)
{
   StrPlug *tempcon = PluggedInto();

   if (p == tempcon)  // If we're already plugged in... (Recursive call)
      return(1);
   else {
      if (p == 0 || p == this)  // If we're being plugged into ourselves, or
	 return(0);             // nothing.
      else if (tempcon != 0)  // If we're already plugged in, and are trying to
	 return(0);          // be plugged into somewhere else.
      else {
	 ConnPlug = p;
	 if (!p->PlugInto(this)) {  // If we didn't succeed in plugging the
	    ConnPlug = tempcon;    // other guy into us....
	    return(0);
	 } else {
	    notify_flags = 0;
	    return(1);
	 }
      }
   }
}

void StrPlug::ReadableNotify()
{
//   cerr << "StrPlug::ReadableNotify()\n";
   StrPlug *mate = PluggedInto();

/*   if (!mate) {
      cerr << "mate failed.\n";
   } else if (!mate->CanRead()) {
      cerr << "mate->CanRead() failed.\n";
   }
   if (!CanWrite()) {
      cerr << "CanWrite() failed.\n";
   } */
   while (mate && mate->CanRead() && CanWrite()) {
      Write(mate->Read());
      mate = PluggedInto();
   }
}

void StrPlug::WriteableNotify()
{
//   cerr << "StrPlug::WriteableNotify()\n";
   StrPlug *mate = PluggedInto();

/*   if (!mate) {
      cerr << "mate failed.\n";
   } else if (!mate->CanWrite()) {
      cerr << "mate->CanWrite() failed.\n";
   }
   if (!CanRead()) {
      cerr << "CanRead() failed.\n";
   } */
   while (mate && mate->CanWrite() && CanRead()) {
      mate->Write(Read());
      mate = PluggedInto();
   }
}
