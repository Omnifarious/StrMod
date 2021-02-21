/* $Rev: 103 $ $URL: https://svn.generalpresence.com:5131/repos/trunk/C++/PortForward/PortForward.cxx $ */

// For a log, see ChangeLog

#include <StrMod/SockListenModule.h>
#include <StrMod/SocketModule.h>
#include <EHnet++/SocketAddress.h>
#include <EHnet++/InetAddress.h>
#include <UniEvent/Dispatcher.h>
#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/UnixEventRegistry.h>
#include <UniEvent/UnixEventPoll.h>
#include <UniEvent/RegistryDispatcherGlue.h>
#include <iostream>
#include <string>
#include <list>
#include <memory>

using strmod::strmod::SockListenModule;
using strmod::strmod::SocketModule;
using strmod::unievent::Dispatcher;
using strmod::unievent::UnixEventRegistry;

// The EHnet++ library has not yet been converted to use namespaces,
// so SocketAddress and InetAddress can be used 'bare'.

class ForwardedConnection {
 public:
   using fdmodptr_t = ::std::unique_ptr<strmod::strmod::StreamFDModule>;
   ForwardedConnection(fdmodptr_t incoming, fdmodptr_t outgoing);
   ~ForwardedConnection();

 private:
   class ErrorEvent : public strmod::unievent::Event {
    public:
      ErrorEvent(ForwardedConnection &parent)
           : parent_(parent), parentgood_(true)
      {
      }
      virtual ~ErrorEvent() {}

      virtual void triggerEvent(Dispatcher *dispatcher = 0) {
         if (parentgood_)
         {
            parent_.errorOccurred();
         }
      }

      void parentGone() { parentgood_ = false; }

    private:
      ForwardedConnection &parent_;
      bool parentgood_;
   };
   friend class ErrorEvent;

   fdmodptr_t const incoming_;
   fdmodptr_t const outgoing_;
   ::std::shared_ptr<ErrorEvent> errev_;

   void errorOccurred();
};

ForwardedConnection::ForwardedConnection(fdmodptr_t incoming,
                                         fdmodptr_t outgoing)
     : incoming_(::std::move(incoming)), outgoing_(::std::move(outgoing)),
       errev_(::std::make_shared<ErrorEvent>(*this))
{
   using strmod::strmod::StreamFDModule;

   incoming_->makePlug(0)->plugInto( *(outgoing_->makePlug(0)) );

   incoming_->onErrorIn(StreamFDModule::ErrRead, errev_);
   incoming_->onErrorIn(StreamFDModule::ErrWrite, errev_);
   incoming_->onErrorIn(StreamFDModule::ErrGeneral, errev_);
   incoming_->onErrorIn(StreamFDModule::ErrFatal, errev_);

   outgoing_->onErrorIn(StreamFDModule::ErrRead, errev_);
   outgoing_->onErrorIn(StreamFDModule::ErrWrite, errev_);
   outgoing_->onErrorIn(StreamFDModule::ErrGeneral, errev_);
   outgoing_->onErrorIn(StreamFDModule::ErrFatal, errev_);
}

ForwardedConnection::~ForwardedConnection()
{
   errev_->parentGone();
}

void ForwardedConnection::errorOccurred()
{
   std::cerr << "Closing down a connection due to an error!\n";
   delete this;
}

//----

class Listener
{
 public:
   // Lets alias a few types that we use all over in this class to make things
   // easier on our fingers.
   typedef ::strmod::ehnet::SocketAddress SocketAddress;
   typedef ::strmod::unievent::Dispatcher Dispatcher;
   typedef ::strmod::unievent::UnixEventRegistry UnixEventRegistry;
   Listener(const SocketAddress &from, const SocketAddress &to,
            Dispatcher &disp, UnixEventRegistry &ureg);
   ~Listener();

   //! Called once a loop from the event dispatching loop.
   void doPoll();

 private:
   ::std::unique_ptr<SockListenModule> listener_;
   SockListenModule::SLPlug *lplug_;
   ::std::unique_ptr<SocketAddress> const from_;
   ::std::unique_ptr<SocketAddress> const to_;
   Dispatcher &disp_;
   UnixEventRegistry &ureg_;
};

Listener::Listener(const SocketAddress &from, const SocketAddress &to,
                   Dispatcher &disp, UnixEventRegistry &ureg)
     : lplug_(nullptr), from_(from.Copy()), to_(to.Copy()),
       disp_(disp), ureg_(ureg)
{
   listener_ = ::std::make_unique<SockListenModule>(*from_, disp, ureg, 2);
   lplug_ = listener_->makePlug();
}

Listener::~Listener()
{
   if (lplug_)
   {
      listener_->deletePlug(lplug_);
      lplug_ = nullptr;
   }
}

void Listener::doPoll()
{
   if (listener_ && listener_->hasError())
   {
      // If the listener has an error, print the error.
      {
         using std::cerr;
         char errbuf[256] = {0};  // Magic number!
         listener_->getError().getErrorString(errbuf, sizeof(errbuf) - 1);
         cerr << "Listening socket has this error: ("
              << listener_->getError().getSyscallName()
              << ") " << errbuf << "\n";
      }

      // Then tear down the listening socket and try to rebuild it.
      if (lplug_)
      {
         listener_->deletePlug(lplug_);
         lplug_ = nullptr;
      }
      listener_.release();
   }
   if (!listener_)
   {
      listener_ = ::std::make_unique<SockListenModule>(*from_, disp_, ureg_, 2);
      lplug_ = listener_->makePlug();
   }
   if (lplug_ && lplug_->isReadable())
   {
      strmod::strmod::SocketModuleChunkPtr smcp = lplug_->getConnection();
      ::std::unique_ptr<SocketModule> incoming{smcp->getModule()};
      if (incoming)
      {
         auto outgoing{::std::make_unique<SocketModule>(*to_, disp_, ureg_, false)};
         new ForwardedConnection(::std::move(incoming), ::std::move(outgoing));
      }
   }
}

typedef std::list<::std::unique_ptr<Listener>> ListenerList;

int main()
{
   using ::strmod::lcore::U2Byte;
   using ::strmod::ehnet::InetAddress;
   strmod::unievent::SimpleDispatcher dispatcher;
   strmod::unievent::UnixEventPoll pollmanager(&dispatcher);
   strmod::unievent::RegistryDispatcherGlue glue(&dispatcher, &pollmanager);
   ListenerList listeners;
   using std::cin;
   using std::cout;
   using std::string;

   do
   {
      U2Byte inport = 0;
      string outaddr;
      unsigned int outport = 0;

      cin >> inport >> outaddr >> outport;
      if (cin)
      {
         InetAddress listenaddr(inport);
         InetAddress destaddr(outaddr, outport);
         // Use push_front to reverse order to delete LIFO
         listeners.push_front(
                 ::std::make_unique<Listener>(
                         listenaddr, destaddr, dispatcher, pollmanager
                         ));
         cout << "Accepting connections on port " << inport
              << " and redirecting data to a connection to "
              << outaddr << " port " << outport << "\n";
      }
   } while (cin);

   while (true)
   {
      dispatcher.dispatchEvent();
      for (auto const &listener: listeners)
      {
         listener->doPoll();
      }
   }

   return 0;
}
