/* $Header$ */

// $Log$
// Revision 1.2  1998/05/01 11:54:33  hopper
// Made various changes so that the UNIXpollManager could automatically
// register itself to be run when the event queue was empty.
// Also, changes to use bool_val and bool_cst instead of bool so that we
// can move to a paltform that doesn't support bool more easily.
//
// Revision 1.1  1998/04/29 01:57:04  hopper
// First cut at making something that can dispatch UNIX system events.
//

#ifdef __GNUG__
#  pragma implementation "UNIXpollManager.h"
#endif

#include "UniEvent/UNIXpollManager.h"

const UNEVT_ClassIdent UNIXpollManager::identifier(7UL);
const UNEVT_ClassIdent UNIXpollManager::PollEvent::identifier(8UL);
