/* $Header$ */

// $Log$
// Revision 1.1  1999/10/15 11:20:06  hopper
// Added a new 'Debugable' interface for checking class invariants and
// printing state.
//

#ifdef __GNUG__
#  pragma implementation "Debugable.h"
#endif

#include "LCore/Debugable.h"

const LCore_ClassIdent Debugable::identifier(10UL);
