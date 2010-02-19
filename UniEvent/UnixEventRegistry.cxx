/* $Header$ */

// $Log$
// Revision 1.2  2002/03/15 05:00:49  hopper
// Added a couple of common static member variables that all subclasses
// will need to share.
//
// Revision 1.1  2002/02/28 22:29:09  hopper
// Initial version.  Just the .cxx stub for an interface.
//

#ifdef __GNUG__
#  pragma implementation "UnixEventRegistry.h"
#endif

#include "UniEvent/UnixEventRegistry.h"

namespace strmod {
namespace unievent {

const unsigned int UnixEventRegistry::max_handled_by_S;
UnixEventRegistry *UnixEventRegistry::handled_by_S[max_handled_by_S] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

} // namespace unievent
} // namespace strmod
