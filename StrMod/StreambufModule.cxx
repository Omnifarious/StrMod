/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:49  hopper
// Initial revision
//
// Revision 0.3  1994/07/18  03:47:22  hopper
// Moved #pragma implementation stuff around so if we ever had gcc on OS2...
// Changed plain old char's to unsigned char's.
//
// Revision 0.2  1994/06/16  14:03:07  hopper
// Fixed various errors and barriers to compilation.
//
// Revision 0.1  1994/06/16  05:15:17  hopper
// Genesis!
//

// $Revision$

#ifndef OS2
#  ifdef __GNUG__
#     pragma implementation "StreambufModule.h"
#  endif

#  include <StrMod/StreambufModule.h>

#  ifndef _StrChunk_H_
#     include <StrMod/StrChunk.h>
#  endif
#else
#  ifdef __GNUG__
#     pragma implementation "strbfmod.h"
#  endif

#  include "strbfmod.h"

#  ifndef _StrChunk_H_
#     include "strchnk.h"
#  endif
#endif

#include <iostream.h>
#include <strstream.h>

StreambufModule::DeletePlug(StrPlug *plug)
{
   if (OwnsPlug(plug)) {
      if (plug->PluggedInto())
	 plug->UnPlug();
      plugcreated = 0;
      return(1);
   } else
      return(0);
}

StreambufModule::~StreambufModule()
{
   DeletePlug(splug);
   delete splug;
}

istrstream *StreambufModule::GetStream(StrChunk *&bufspace)
{
   bufspace = 0;
   if (!CanGet())
      return(0);

   StrPlug *oplug = splug->PluggedInto();

   assert(oplug != 0);

   StrChunk *chnk = oplug->Read();
   DataBlockStrChunk *dchnk = 0;

   assert(chnk != 0);

   if (!(chnk->IsChunkType(EHChunk::DataBlockStrChunk))) {
      dchnk = new DataBlockStrChunk(chnk->Length(), chnk->GetCVoidP());
      delete chnk;
      chnk = 0;
   } else {
      dchnk = (DataBlockStrChunk *)(chnk);
      chnk = 0;
   }

   int len = dchnk->Length();
   unsigned char *buf = dchnk->GetCharP();
   int i;

   for (i = 0; i < len; i++) {
      if (buf[i] == '\0')
	 buf[i] = nullreplace;
      else
	 assert(buf[i] != nullreplace);
   }

   istrstream *retval = new istrstream((char *)(dchnk->GetCharP()),
				       dchnk->Length());

   bufspace = dchnk;
   return(retval);
}

int StreambufModule::PutStream(ostrstream &os)
{
   if (!CanPut())
      return(0);

   StrPlug *oplug = splug->PluggedInto();

   assert(oplug != 0);

   unsigned char *str = (unsigned char *)(os.str());

   assert(str != 0 && os.pcount() > 0);

   DataBlockStrChunk *dchnk = new DataBlockStrChunk(os.pcount(), str);

   int len = dchnk->Length();
   int i;

   str = dchnk->GetCharP();

   for (i = 0; i < len; i++) {
      if (str[i] == nullreplace)
	 str[i] = '\0';
   }

   assert(oplug->Write(dchnk));

   return(1);
}

int StreambufModule::PutStream(ostrstream *os)
{
   if (PutStream(*os)) {
      delete os;
      return(1);
   } else
      return(0);
}
