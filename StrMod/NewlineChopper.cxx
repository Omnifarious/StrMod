/* $Header$ */

// For a log, see ChangeLog
//
// Revision 1.1  1996/09/02 23:18:40  hopper
// Added NewlineChopper class so users would have a simple class that
// would break up and recombine streams using '\n' as a separator.
//

#ifdef __GNUG__
#  pragma implementation "NewlineChopper.h"
#endif

#include <StrMod/NewlineChopper.h>

const STR_ClassIdent NewlineChopper::identifier(27UL);
