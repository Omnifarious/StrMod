#ifndef _UNEVT_RegistryDispatcherGlue_H_  // -*-c++-*-


/* $Header$ */

// For a log, see ../ChangeLog

#define _UNEVT_RegistryDispatcherGlue_H_

#include <memory>
#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>

namespace strmod {
namespace unievent {

class Dispaatcher;
class UnixEventRegistry;

class RegistryDispatcherGlue
{
 public:
   RegistryDispatcherGlue(Dispatcher *disp, UnixEventRegistry *ureg);
   virtual ~RegistryDispatcherGlue();

   void doBusyAction();
   void doQEmptyAction();

   Dispatcher *dispatcher() const                              { return disp_; }
   void dispatcher(Dispatcher *disp);
   UnixEventRegistry *eventRegistry() const                    { return ureg_; }
   void eventRegistry(UnixEventRegistry *ureg)                 { ureg_ = ureg; }

 private:
   class EmptyEvent : public Event
   {
    public:
      EmptyEvent(RegistryDispatcherGlue *parent) : parent_(parent)           { }
      virtual ~EmptyEvent() = default;

      inline void triggerEvent(Dispatcher * = 0) override
      {
         if (parent_)
         {
            parent_->doQEmptyAction();
         }
      }

      void parentGone()                                         { parent_ = 0; }

    private:
      RegistryDispatcherGlue *parent_;
   };
   class BusyEvent : public Event
   {
    public:
      BusyEvent(RegistryDispatcherGlue *parent) : parent_(parent)            { }
      virtual ~BusyEvent() = default;

      inline void triggerEvent(Dispatcher * = 0) override
      {
         if (parent_)
         {
            parent_->doBusyAction();
         }
      }

      void parentGone()                                         { parent_ = 0; }

    private:
      RegistryDispatcherGlue *parent_;
   };
   Dispatcher *disp_;
   UnixEventRegistry *ureg_;
   ::std::shared_ptr<EmptyEvent> emptyev_;
   ::std::shared_ptr<BusyEvent> busyev_;
};

} // namespace unievent
} // namespace strmod

#endif
