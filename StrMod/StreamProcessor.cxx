/* $Header$ */

// See Changelog for a log of changes.
//
// Revision 1.1  1996/09/02 23:33:39  hopper
// Created abstract class (class StreamProcessor) for handling simple
// dataflows where one grabbed incoming data, did stuff to it, and sent
// the result along.
//

#ifdef __GNUG__
#  pragma implementation "StreamProcessor.h"
#endif

#include "StrMod/StreamProcessor.h"
#include "StrMod/StrChunkPtr.h"

const STR_ClassIdent StreamProcessor::identifier(25UL);

StreamProcessor::~StreamProcessor()
{
   // outgoing_ and incoming_ are smart pointers and will automagically handle
   // the deletion themselves.
}
