/* $Header$ */

// $Log$
// Revision 1.1  1997/05/13 01:03:25  hopper
// Added new EventPtr classes to maintain reference count on UNIEvent classes.
//

#ifdef __GNUG__
#  pragma implementation "EventPtr.h"
#endif

#include <UniEvent/EventPtr.h>

#ifdef __GNUG__
#  pragma implementation "EventPtrT.h"
#endif

#include <UniEvent/EventPtrT.h>

const UNEVT_ClassIdent UNIEventPtr::identifier(5UL);
