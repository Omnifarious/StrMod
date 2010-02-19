/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "RegistryDispatcherGlue.h"
#endif

#include "UniEvent/RegistryDispatcherGlue.h"
#include "UniEvent/Dispatcher.h"
#include "UniEvent/UnixEventRegistry.h"

namespace strmod {
namespace unievent {

RegistryDispatcherGlue::RegistryDispatcherGlue(Dispatcher *disp,
                                               UnixEventRegistry *ureg)
     : disp_(disp), ureg_(ureg),
       emptyev_(new EmptyEvent(this)), busyev_(new BusyEvent(this))
{
   if (disp_)
   {
      disp_->onQueueEmpty(emptyev_);
      disp_->addBusyPollEvent(busyev_);
   }
}

RegistryDispatcherGlue::~RegistryDispatcherGlue()
{
   emptyev_->parentGone();
   busyev_->parentGone();
}

void RegistryDispatcherGlue::doBusyAction()
{
   if (disp_)
   {
      disp_->addBusyPollEvent(busyev_);
   }
   if (ureg_)
   {
      ureg_->doPoll(false);
   }
}

void RegistryDispatcherGlue::doQEmptyAction()
{
   if (disp_)
   {
      disp_->onQueueEmpty(emptyev_);
   }
   if (ureg_)
   {
      ureg_->doPoll(true);
   }
}

void RegistryDispatcherGlue::dispatcher(Dispatcher *disp)
{
   if (disp != disp_)
   {
      emptyev_->parentGone();
      emptyev_.ReleasePtr();
      busyev_->parentGone();
      busyev_.ReleasePtr();

      disp_ = disp;
      emptyev_ = new EmptyEvent(this);
      busyev_ = new BusyEvent(this);

      if (disp_)
      {
         disp_->onQueueEmpty(emptyev_);
         disp_->addBusyPollEvent(busyev_);
      }
   }
}

} // namespace unievent
} // namespace strmod
