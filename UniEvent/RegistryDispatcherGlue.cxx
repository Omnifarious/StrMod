/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "RegistryDispatcherGlue.h"
#endif

#include "UniEvent/RegistryDispatcherGlue.h"

namespace strmod {
namespace unievent {

RegistryDispatcherGlue::RegistryDispatcherGlue(Dispatcher *disp,
                                               UnixEventRegistry *ureg)
     : disp_(disp), ureg_(ureg),
       emptyev_(new EmptyEvent(this)), busyev_(new BusyEvent(this))
{
}

RegistryDispatcherGlue::~RegistryDispatcherGlue()
{
   emptyev_->parentGone();
   busyev_->parentGone();
}

} // namespace unievent
} // namespace strmod
