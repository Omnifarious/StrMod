/* $Header$ */

// $Log$
// Revision 1.5  1996/09/02 23:15:47  hopper
// Added new classes to class id comment list.
//
// Revision 1.4  1996/08/24 12:59:53  hopper
// Added comments for new SimplePlug, and SimpleMultiplexer classes.
//
// Revision 1.3  1996/06/29 06:43:42  hopper
// Changed comment style to be more rational and added comments for new
// classes.
//
// Revision 1.2  1996/05/08 11:11:29  hopper
// Added a couple of new classes to the comments section.
//
// Revision 1.1.1.1  1995/07/22 04:46:49  hopper
// Imported sources
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
   0UL	STR_ClassIdent
   1UL	StreamModule
   2UL	StrPlug
   3UL	EchoModule
   4UL	EchoPlug
   6UL	StrChunk
   7UL	DataBlockStrChunk
   8UL	StreamFDModule
   9UL	StrFDPlug
  10UL	SplitterModule
  11UL	SplitterPlug
  12UL	SocketModule
  13UL	SockListenModule
  14UL	ListeningPlug
  15UL	SocketModuleChunk
  16UL	GroupVector
  17UL	GroupVector::Iterator
  18UL	StrSubChunk
  19UL  StrChunkPtr
  20UL  GroupChunk
  21UL	SimpleMultiplexer
  22UL	SimpleMultiplexer::MultiPlug
  23UL	SimpleMultiplexer::SinglePlug
  24UL  SimplePlug
  25UL	StreamProcessor
  26UL	CharChopper
  27UL	NewlineChopper
  28UL	PassThrough
  29UL	ProcessorModule
  30UL	ProcessorModule::Plug
*/
