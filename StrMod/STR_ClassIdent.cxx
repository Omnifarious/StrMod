/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:49  hopper
// Initial revision
//
// Revision 1.4  1995/04/18  18:37:57  hopper
// Added comments for new class numbers
//
// Revision 1.3  1995/04/17  22:56:51  hopper
// Added comment for SocketModule class.
//
// Revision 1.2  1995/04/14  16:45:19  hopper
// Changed to use new include scheme for libraries.
// Added comments about the numbers used for other classes in the library.
//
// Revision 1.1  1995/03/08  05:37:00  hopper
// Initial revision
//

#ifdef __GNUG__
#  pragma implementation "STR_ClassIdent.h"
#endif

#include "StrMod/STR_ClassIdent.h"

const STR_ClassIdent STR_ClassIdent::identifier(0UL);

/*
   STR_ClassIdent	0UL
   StreamModule		1UL
   StrPlug		2UL
   EchoModule		3UL
   EchoPlug		4UL
   StrChunkBuffer	5UL
   StrChunk		6UL
   DataBlockStrChunk	7UL
   StreamFDModule	8UL
   StrFDPlug		9UL
   SplitterModule	10UL
   SplitterPlug		11UL
   SocketModule		12UL
   SockListenModule	13UL
   ListeningPlug	14UL
   SocketModuleChunk	15UL
*/
