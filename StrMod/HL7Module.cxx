/* $Header$ */

#ifdef __GNUG__
#  pragma implementation "HL7Module.h"
#endif

// $Log$
// Revision 1.1  1995/07/22 04:46:49  hopper
// Initial revision
//
// Revision 0.4  1995/01/09  05:23:10  hopper
// Changed the state machine that matched message seperators into an
// implementation of the KMP (Knuth Morris Pratt) string matching algorithm.
//
// Revision 0.3  1994/07/18  03:43:21  hopper
// Changed to use unsigned char's instead of normal char's.
//
// Revision 0.2  1994/06/21  16:03:39  hopper
// Fixed a small error in which the state wasn't preserved across message
// boundries. Also, added lot's of commented out debugging statements.
//
// Revision 0.1  1994/06/21  02:57:39  hopper
// Genesis!
//

// $Revision$

#ifndef OS2
#   include <StrMod/HL7Module.h>

#   ifndef _StrChunk_H_
#      include <StrMod/StrChunk.h>
#   endif
#else
#   include "hl7mod.h"
#   include <mem.h>
#   ifndef _StrChunk_H_
#      include "strchnk.h"
#   endif
#endif

#include <string.h>

// #include <iostream.h>

void HL7Module::SetSeperator(const char *s)
{
   int len = strlen(s);

   assert(len >= 1 && len <= 127);

   strcpy(seperator, s);
   seplen = (unsigned short)(len);

   prefixtable[0] = 0;

   int prefixpos = 0;
   int i;

   for (i = 1; i < len; i++) {
      while ((prefixpos > 0) && (s[prefixpos] != s[i]))
         prefixpos = prefixtable[prefixpos - 1];
      if (s[prefixpos] == s[i])
         prefixpos++;
      prefixtable[i] = (unsigned short)(prefixpos);
   }
}

void HL7Module::DoProcessing()
{
   assert(processing != 0);
   assert(message == 0 || nexttest < seplen);

   int len = processing->Length();
   unsigned int bufpos = bytesprocessed;
   unsigned short curnext = nexttest;
   const unsigned short curlen = seplen;

   assert(bufpos < len);
   assert(nexttest >= 0 && nexttest <= curlen);

   unsigned char *buf = processing->GetCharP();

   if (curnext >= curlen)
      curnext = prefixtable[curnext - 1];
   do {
//      cerr << "oldstate == " << curstate
//	   << " && buf[bufpos] == '" << buf[bufpos];
      while ((curnext > 0) && (seperator[curnext] != buf[bufpos]))
         curnext = prefixtable[curnext - 1];
      if (seperator[curnext] == buf[bufpos])
         curnext++;
      bufpos++;
//      curstate = ChangeState(curstate, buf[bufpos++]);
//      cerr << "' && newstate == " << curstate << '\n';
   } while (bufpos < len && curnext < curlen);
   if (message == 0) {
      message = new DataBlockStrChunk(bufpos - bytesprocessed,
				      buf + bytesprocessed);
   } else {
      unsigned int start = message->Length();

      message->Resize(start + (bufpos - bytesprocessed));
      memcpy(message->GetCharP() + start,
	     buf + bytesprocessed, bufpos - bytesprocessed);
   }
   if (curnext >= curlen)
      messagedone = 1;
   else
      messagedone = 0;
   if (bufpos >= len) {
      delete processing;
      processing = 0;
      bytesprocessed = 0;
   } else
      bytesprocessed = bufpos;

   nexttest = curnext;

#ifndef NDEBUG
   if (messagedone)
      assert(message != 0);
   else
      assert(processing == 0);
#endif
}

int HL7Module::DeletePlug(StrPlug *plug)
{
   if (OwnsPlug(plug)) {
      if (plug->PluggedInto())
	 plug->UnPlug();
      if (plug == plug0)
	 created.zero = 0;
      else
	 created.one = 0;
      return(1);
   } else
      return(0);
}

HL7Module::~HL7Module()
{
   DeletePlug(plug0);
   DeletePlug(plug1);
   delete plug0;
   delete plug1;
}


void HL7Plug::ReadableNotify()
{
   HL7Module *parent = ModuleFrom();

//   cerr << "HL7Plug::ReadableNotify() called! && side == " << side << '\n';
//   if (side == 0) {
//      cerr << "parent->created.zero == " << parent->created.zero;
//      cerr << " && CanWrite() == " << CanWrite() << '\n';
//   }
   if (side != 0 && parent->created.one && parent->created.zero)
      parent->plug0->DoReadableNotify();
   else if (side == 0 && parent->created.zero) {
      StrPlug *oplug = PluggedInto();

      while (oplug && CanWrite() && oplug->CanRead()) {
	 StrChunk *curchnk;

	 curchnk = oplug->Read();
	 Write(curchnk);
	 oplug = PluggedInto();
      }
   }
}

void HL7Plug::WriteableNotify()
{
   HL7Module *parent = ModuleFrom();

   if (side == 0 && parent->created.zero && parent->created.one)
      parent->plug1->DoWriteableNotify();
   else if (side == 1 && parent->created.one) {
      StrPlug *oplug = PluggedInto();

      while (oplug && CanRead() && oplug->CanWrite()) {
	 StrChunk *curchnk;

	 curchnk = Read();
	 oplug->Write(curchnk);
	 oplug = PluggedInto();
      }
   }
}

int HL7Plug::CanWrite() const
{
   HL7Module *parent = ModuleFrom();

   if (side != 0) {
      StrPlug *oplug;

      if (parent->created.one && parent->created.zero
	  && (oplug = parent->plug0->PluggedInto()) != 0) {
	 return(oplug->CanWrite());
      } else
	 return(0);
   } else {
      return(parent->created.zero &&
	     !ioflags.wrtngto && parent->processing == 0);
   }
}

int HL7Plug::Write(StrChunk *chnk)
{
   HL7Module *parent = ModuleFrom();

   if (side != 0) {
      StrPlug *oplug;

      if (parent->created.zero && (oplug = parent->plug0->PluggedInto()) != 0)
	 return(oplug->Write(chnk));
      else
	 return(0);
   } else {
      if (CanWrite()) {
	 if (chnk->IsChunkType(EHChunk::DataBlockStrChunk))
	    parent->processing = (DataBlockStrChunk *)(chnk);
	 else {
	    unsigned int len = chnk->Length();

	    parent->processing = new DataBlockStrChunk(len, chnk->GetCVoidP());
	    delete chnk;
	 }
	 chnk = 0;
	 if (!(parent->messagedone)) {
	    parent->DoProcessing();
	    if (parent->messagedone && parent->created.one) {
	       ioflags.wrtngto = 1;
	       if (parent->plug1->ioflags.rdngfrm == 0)
		  parent->plug1->DoReadableNotify();
	       ioflags.wrtngto = 0;
	    }
	 }
	 return(1);
      } else
	 return(0);
   }
}

int HL7Plug::CanRead() const
{
   HL7Module *parent = ModuleFrom();

//   cerr << "HL7Plug::CanRead() const called! && side == " << side << '\n';
   if (side == 0) {
      StrPlug *oplug;

      if (parent->created.zero && parent->created.one &&
	  (oplug = parent->plug1->PluggedInto()) != 0) {
	 return(oplug->CanRead());
      } else
	 return(0);
   } else
      return(parent->created.one && !ioflags.rdngfrm && parent->messagedone);
}

StrChunk *HL7Plug::InternalRead()
{
   HL7Module *parent = ModuleFrom();

   if (side == 0) {
      StrPlug *oplug;

      if (parent->created.one && (oplug = parent->plug1->PluggedInto()) != 0)
	 return(oplug->Read());
      else
	 return(0);
   } else {
      if (CanRead()) {
	 StrChunk *msg = parent->message;

	 parent->messagedone = 0;
	 parent->message = 0;
	 if (parent->processing) {
	    parent->DoProcessing();
	    if (parent->processing == 0 && parent->created.zero) {
	       ioflags.rdngfrm = 1;
	       if (parent->plug0->ioflags.wrtngto == 0)
		  parent->plug0->DoWriteableNotify();
	       ioflags.rdngfrm = 0;
	    }
	 }
	 return(msg);
      } else
	 return(0);
   }
}
