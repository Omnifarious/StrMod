/* $Header$ */

 // $Log$
 // Revision 1.4  1996/07/05 18:47:14  hopper
 // Changed to use new StrChunkPtr stuff.
 //
 // Revision 1.3  1996/02/12 05:49:23  hopper
 // Changed to use new ANSI string class instead of RWCString.
 //
 // Revision 1.2  1995/07/23 04:00:34  hopper
 // Added #include <string.h> in various places to avoid triggering a bug
 // in libg++ 2.7.0
 //
 // Revision 1.1.1.1  1995/07/22 04:46:48  hopper
 // Imported sources
 //
 // -> Revision 0.17  1995/04/14  16:32:33  hopper
 // -> Combined versions 0.16 and 0.16.0.4
 // ->
 // -> Revision 0.16.0.4  1995/04/14  16:31:42  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.16.0.3  1994/07/18  03:44:40  hopper
 // -> Added #pragma implementation thing so this would work better with gcc 2.6.0
 // ->
 // -> Revision 0.16.0.2  1994/05/17  06:09:43  hopper
 // -> Added declaration for strerror that g++ seems to want.
 // ->
 // -> Revision 0.16.0.1  1994/05/16  06:08:10  hopper
 // -> Changed to RWCString instead of my String class.
 // ->
 // -> Revision 0.16  1994/05/16  06:06:17  hopper
 // -> Changed to use ANSI error string lookup stuff.
 // ->
 // -> Revision 0.15  1994/05/07  03:24:53  hopper
 // -> Changed header files stuff around to be aprroximately right with
 // -> new libraries, and new names & stuff.
 // ->
 // -> Revision 0.14  1992/05/31  21:51:53  hopper
 // -> Made couple of modifications to get it to work on an SGI.
 // ->
 // -> Revision 0.13  1992/05/14  21:10:17  hopper
 // -> Forgot to initialize max_block_size in
 // -> StreamFDModule::StreamFDModule(int, unsigned int, unsigned int, unsigned int
 // ->                                unsigned int)
 // ->
 // -> Revision 0.12  1992/05/02  01:02:34  hopper
 // -> Added check for errno if fd passed in constructor is < 0
 // ->
 // -> Revision 0.11  1992/04/24  00:25:41  hopper
 // -> Added support for a special error (EOF) which has # -1
 // ->
 // -> Revision 0.10  1992/04/15  02:33:28  hopper
 // -> Gensis!
 // ->

#ifdef __GNUG__
#  pragma implementation "StreamFDModule.h"
#endif

#include "StrMod/StreamFDModule.h"
#ifndef _STR_DBStrChunk_H_
#   include "StrMod/DBStrChunk.h"
#endif

extern "C" int sys_nerr;
extern "C" int errno;
extern "C" int close(int);

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>

#ifndef NO_RcsID
static char StreamFDModule_CC_rcsID[] =
    "$Id$";
#endif

extern "C" char *strerror(int);

const STR_ClassIdent StreamFDModule::identifier(8UL);
const STR_ClassIdent StrFDPlug::identifier(9UL);

string StreamFDModule::ErrorString() const
{
   int errn = ErrorNum();

   if (errn == 0)
      return("no error at all, nothing happened, nope :-)");
   else if (errn > 0) {
      char *err = strerror(errn);

      if (err)
	 return(err);
      else
	 return("unknown error");
   } else if (errn == -1)
      return("end of file encountered");
   else
      return("unknown error");
}

int StreamFDModule::BestChunkSize()
{
#ifndef IRIS
   if (fd >= 0) {
      struct stat sbuf;

      if (fstat(fd, &sbuf) < 0) {
	 last_error = errno;
	 if (errno == EBADF) {
	    fd = -1;
	    return(0);
	 } else
	    return(4096);   // Nice default size. BUFSIZ in stdio.h on Sun's is
                            // too small.
      }
      return(sbuf.st_blksize);
   } else
      return(0);
#else
   return(4096);
#endif
}

StreamFDModule::StreamFDModule(int fdes, IOCheckFlags flgs, bool hangdel)
     : fd(fdes), plug(0), last_error(0),
       write_pos(0), write_vec(0), max_block_size(0)
{
   if (fd < 0)
      last_error = errno;
   flags.hangdelete = (hangdel ? 1 : 0);
   flags.plugmade = 0;
   switch (flgs) {
    default:
    case CheckNone:
      flags.checkread = 0;
      flags.checkwrite = 0;
      break;
    case CheckRead:
      flags.checkread = 1;
      flags.checkwrite = 0;
      break;
    case CheckWrite:
      flags.checkread = 0;
      flags.checkwrite = 1;
      break;
    case CheckBoth:
      flags.checkread = 1;
      flags.checkwrite = 1;
      break;
   }
   plug = new StrFDPlug(this);
}

StreamFDModule::~StreamFDModule()
{
   if (flags.hangdelete && !plug->CanWrite())
      plug->Write(new DataBlockStrChunk());  // Flush the buffer by writing 0
   delete plug;                              // length output buffer.
   close(fd);
}
