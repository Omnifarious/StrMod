/* $Header$ */

// For a log, see Changelog
//
// Revision 1.1  1996/09/02 23:26:41  hopper
// Added PassThrough class to use when you needed a StreamProcessor that
// did nothing except buffer one Chunk worth of data.
//

#ifdef __GNUG__
#  pragma implementation "PassThrough.h"
#endif

#include <StrMod/PassThrough.h>

const STR_ClassIdent PassThrough::identifier(28UL);
