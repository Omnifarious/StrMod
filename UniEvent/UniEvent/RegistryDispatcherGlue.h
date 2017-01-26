#ifndef _UNEVT_RegistryDispatcherGlue_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#define _UNEVT_RegistryDispatcherGlue_H_

#include <tr1/memory>
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
      virtual ~EmptyEvent()                                     { }

      inline virtual void triggerEvent(Dispatcher *dispatcher = 0)
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
      virtual ~BusyEvent()                                      { }

      inline virtual void triggerEvent(Dispatcher *dispatcher = 0)
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
   ::std::tr1::shared_ptr<EmptyEvent> emptyev_;
   ::std::tr1::shared_ptr<BusyEvent> busyev_;
};

} // namespace unievent
} // namespace strmod

#endif