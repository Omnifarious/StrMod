#ifndef _UNEVT_RegistryDispatcherGlue_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#define _UNEVT_RegistryDispatcherGlue_H_

#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>
#include <UniEvent/EventPtrT.h>

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
   UnixEventRegistry *evenRegistry() const                     { return ureg_; }
   void evenRegistry(UnixEventRegistry *ureg);

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
   EventPtrT<EmptyEvent> emptyev_;
   EventPtrT<BusyEvent> busyev_;
};

} // namespace unievent
} // namespace strmod

#endif
