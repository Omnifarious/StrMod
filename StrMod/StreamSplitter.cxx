/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/22 04:46:48  hopper
 // Initial revision
 //
 // -> Revision 0.12  1995/04/14  16:28:32  hopper
 // -> Combined version 0.11 and 0.11.0.6
 // ->
 // -> Revision 0.11.0.6  1995/04/14  16:11:22  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.11.0.5  1994/07/18  03:45:28  hopper
 // -> Added #pragma implementation thing so this would work better with gcc 2.6.0
 // -> Removed own definition of the C function exirt.
 // ->
 // -> Revision 0.11.0.4  1994/06/21  03:42:55  hopper
 // -> Fixed another INCREDIBLY STUPID error in the CanRead function.
 // ->
 // -> Revision 0.11.0.3  1994/06/12  03:59:06  hopper
 // -> Fixed an INCREDIBLY STUPID error in the Write function.
 // ->
 // -> Revision 0.11.0.2  1994/05/26  11:33:39  hopper
 // -> Added some debugging bracketed by
 // -> #ifdef TRACING
 // -> #endif
 // -> directives.
 // ->
 // -> Revision 0.11.0.1  1994/05/16  06:14:03  hopper
 // -> No changes, but I needed a head for the inevitable WinterFire-OS/2
 // -> branch.
 // ->
 // -> Revision 0.11  1994/05/07  03:24:53  hopper
 // -> Changed header files stuff around to be aprroximately right with
 // -> new libraries, and new names & stuff.
 // ->
 // -> Revision 0.10  1992/04/27  21:33:41  hopper
 // -> Genesis!
 // ->

#ifndef NO_RcsID
static char _EH_StreamSplitter_CC_rcsID[] =
      "$Id$";
#endif

#ifdef __GNUG__
#  pragma implementation "StreamSplitter.h"
#endif

#include "StrMod/StreamSplitter.h"

#ifndef _STR_StrChunk_H_
#   include "StrMod/StrChunk.h"
#endif

#include <iostream.h>
#include <Dispatch/dispatcher.h>

const STR_ClassIdent SplitterModule::identifier(10UL);
const STR_ClassIdent SplitterPlug::identifier(11UL);

StrPlug *SplitterModule::CreatePlug(int side)
{
   if (!(side >= CombinedPlug && side <= OutputOnlyPlug))
      return(0);
   else {
      PlugTypes ptype = PlugTypes(side);

      switch (side) {
       case CombinedPlug:
	 if (combined && !created.combined) {
	    created.combined = 1;
	    return(combined);
	 } else
	    return(0);
	 break;
       case InputOnlyPlug:
	 if (inputonly && !created.inputonly) {
	    created.inputonly = 1;
	    return(inputonly);
	 } else
	    return(0);
	 break;
       case OutputOnlyPlug:
	 if (outputonly && !created.outputonly) {
	    created.outputonly = 1;
	    return(outputonly);
	 } else
	    return(0);
	 break;
       default:
	 assert(0 && "Unreachable code!");
	 return(0);
	 break;
      }
   }
   assert(0 && "Unreachable code!");
}

bool SplitterModule::DeletePlug(StrPlug *plug)
{
   if (created.combined && plug == combined) {
      created.combined = 0;
      if (combined->PluggedInto())
	 combined->UnPlug();
      return(true);
   } else if (created.inputonly && plug == inputonly) {
      created.inputonly = 0;
      if (inputonly->PluggedInto())
	 inputonly->UnPlug();
      return(true);
   } else if (created.outputonly && plug == outputonly) {
      created.outputonly = 0;
      if (outputonly->PluggedInto())
	 outputonly->UnPlug();
      return(true);
   } else
      return(false);
}

SplitterModule::SplitterModule()
{
   created.combined = created.inputonly = created.outputonly = 0;

   combined = new SplitterPlug(this, CombinedPlug);
   inputonly = new SplitterPlug(this, InputOnlyPlug);
   outputonly = new SplitterPlug(this, OutputOnlyPlug);
}

SplitterModule::~SplitterModule()
{
   if (created.combined)
      DeletePlug(combined);
   if (created.inputonly)
      DeletePlug(inputonly);
   if (created.outputonly)
      DeletePlug(outputonly);
   delete combined;
   delete inputonly;
   delete outputonly;
}

StrChunk *SplitterPlug::InternalRead()
{
   SplitterModule *parent = ModuleFrom();

#ifdef TRACING
   cerr << "SplitterPlug::InternalRead()\n";
   cerr << "PlugType() == " << PlugType() << "\n";
#endif
   if (PlugType() == SplitterModule::InputOnlyPlug) {
      cerr << "A SplitterPlug received a read request from a StreamModule ";
      cerr << "that wasn't supposed\nto be trying to read from it.\n";
      cerr << "(i.e. an OutputOnlyPlug got a read request.\n";
      cerr << "*cough* *cough*\n";
      exit(23);
      assert(0 && "Unreachable code!");
      return(0);  // Control should never reach here. This is just to shut up
   } else {       //  the compiler.
      SplitterPlug *checkplug;

      if (PlugType() == SplitterModule::OutputOnlyPlug)
	 checkplug = parent->combined;
      else
	 checkplug = parent->inputonly;

      StrChunk *temp = 0;

      if (checkplug->PluggedInto() != 0)
	 temp = checkplug->PluggedInto()->Read();

#ifdef TRACING
      cerr << "temp == \"";
      temp->PutIntoFd(2);
      cerr << "\"\n";
#endif
      return(temp);
   }
}      

bool SplitterPlug::CanWrite() const
{
   SplitterModule *parent = ModuleFrom();

#ifdef TRACING
   cerr << "SplitterPlug::CanWrite()\n";
   cerr << "PlugType() == " << PlugType() << "\n";
#endif
   switch (PlugType()) {
    case SplitterModule::CombinedPlug:
      if (!parent->created.combined)
	 return(0);
      if (parent->created.outputonly && parent->outputonly->PluggedInto())
	 return(parent->outputonly->PluggedInto()->CanWrite());
      else
	 return(0);
    case SplitterModule::InputOnlyPlug:
      if (!parent->created.inputonly)
	 return(0);
      if (parent->created.combined && parent->combined->PluggedInto())
	 return(parent->combined->PluggedInto()->CanWrite());
      else
	 return(0);
    case SplitterModule::OutputOnlyPlug:
    default:
      return(0);
   }
}

bool SplitterPlug::Write(StrChunk *chunk)
{
#ifdef TRACING
   cerr << "SplitterPlug::Write(StrChunk *chunk)\n";
   cerr << "PlugType() == " << PlugType() << "\n";
   cerr << "chunk == \"";
   chunk->PutIntoFd(2);
   cerr << "\"\n";
#endif
   SplitterModule *parent = ModuleFrom();

   if (PlugType() == SplitterModule::OutputOnlyPlug) {
      if (!parent->created.outputonly)
	 return(false);
      else {
	 delete chunk;
	 return(true);
      }
   } else {
      SplitterPlug *checkplug;

      if (PlugType() == SplitterModule::InputOnlyPlug) {
	 if (!parent->created.inputonly)
	    return(false);
	 checkplug = parent->combined;
      } else {
	 if (!parent->created.combined)
	    return(false);
	 checkplug = parent->outputonly;
      }
      if (checkplug->PluggedInto() == 0) {
	 delete chunk;
	 return(true);
      } else
	 return(checkplug->PluggedInto()->Write(chunk));
   }
}

bool SplitterPlug::CanRead() const
{
   SplitterModule *parent = ModuleFrom();

#ifdef TRACING
   cerr << "SplitterPlug::CanRead()\n";
   cerr << "PlugType() == " << PlugType() << "\n";
#endif
   switch (PlugType()) {
    case SplitterModule::CombinedPlug:
      if (!parent->created.combined)
	 return(false);
      if (parent->created.inputonly && parent->inputonly->PluggedInto())
	 return(parent->inputonly->PluggedInto()->CanRead());
      else
	 return(false);
    case SplitterModule::OutputOnlyPlug:
      if (!parent->created.inputonly)
	 return(false);
      if (parent->created.combined && parent->combined->PluggedInto())
	 return(parent->combined->PluggedInto()->CanRead());
      else
	 return(false);
    case SplitterModule::InputOnlyPlug:
    default:
      return(false);
   }
}

void SplitterPlug::ReadableNotify()
{
#ifdef TRACING
   cerr << "SplitterPlug::ReadableNotify()\n";
   cerr << "PlugType() == " << PlugType() << "\n";
#endif
   if (PlugType() == SplitterModule::CombinedPlug)
      ModuleFrom()->outputonly->DoReadableNotify();
   else if (PlugType() == SplitterModule::InputOnlyPlug)
      ModuleFrom()->combined->DoReadableNotify();
}

void SplitterPlug::WriteableNotify()
{
#ifdef TRACING
   cerr << "SplitterPlug::Writeable()\n";
   cerr << "PlugType() == " << PlugType() << "\n";
#endif
   if (PlugType() == SplitterModule::CombinedPlug)
      ModuleFrom()->inputonly->DoWriteableNotify();
   else if (PlugType() == SplitterModule::OutputOnlyPlug)
      ModuleFrom()->combined->DoWriteableNotify();
}
