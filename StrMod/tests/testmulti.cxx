#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include "StrMod/SimpleMulti.h"
// #include "StrMod/ProcessorModule.h"
// #include "StrMod/NewlineChopper.h"
// #include "StrMod/PassThrough.h"
#include <EHnet++/InetAddress.h>
#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/UNIXpollManagerImp.h>
#include <vector>
#include <iostream>

extern "C" int atoi(const char *);

//  class MyProcessor : public ProcessorModule {
//   public:
//     enum Sides { ToNet, ToMulti };

//     inline MyProcessor();

//     inline bool CanCreate(Sides side) const;
//     inline Plug *MakePlug(Sides side);

//   private:
//     NewlineChopper chopper_;
//     PassThrough passthrough_;
//  };

//  MyProcessor::MyProcessor() : ProcessorModule(&chopper_, &passthrough_)
//  {
//  }

//  inline bool MyProcessor::CanCreate(Sides side) const
//  {
//     return(ProcessorModule::CanCreate((side == ToNet) ? OneSide : OtherSide));
//  }

//  inline MyProcessor::Plug *MyProcessor::MakePlug(Sides side)
//  {
//     return(ProcessorModule::MakePlug((side == ToNet) ? OneSide : OtherSide));
//  }

struct EchoConnection {
   static SimpleMultiplexer *multip;
   static UNISimpleDispatcher disp;

   SocketModule *socket;
//   MyProcessor proc;
   StreamModule::Plug *multiplug;

   inline SimpleMultiplexer *multiplexer();

   inline EchoConnection(SocketModule *sock);
   inline virtual ~EchoConnection();

 private:
   void makeMultiplexer();
};

UNISimpleDispatcher EchoConnection::disp;
SimpleMultiplexer *EchoConnection::multip = 0;

inline SimpleMultiplexer *EchoConnection::multiplexer()
{
   if (multip == 0) {
      makeMultiplexer();
   }
   return(multip);
}

inline EchoConnection::EchoConnection(SocketModule *sock) : socket(sock)
{
   cerr << "Here 1!\n";
   StreamModule::Plug *p1 = socket->makePlug(0);
   StreamModule::Plug *p2
      = multiplexer()->makePlug(SimpleMultiplexer::MultiSide);
   p1->plugInto(*p2);
   cerr << "Here 2!\n";
   multiplug = p2;
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
   multiplug->getParent().deletePlug(multiplug);
}

void EchoConnection::makeMultiplexer()
{
   EchoModule *echom = new EchoModule();
   multip = new SimpleMultiplexer(disp);
   StreamModule::Plug *p1 = echom->makePlug();
   StreamModule::Plug *p2 = multip->makePlug(SimpleMultiplexer::SingleSide);

   p1->plugInto(*p2);
}

template class vector<EchoConnection *>;
typedef vector<EchoConnection *> ConAry;

static void MaintainList(ConAry &lst);

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <port #>\n";
      return(1);
   }

   UNIXpollManagerImp upoll(&EchoConnection::disp);
   InetAddress here(atoi(argv[1]));
   SockListenModule slm(here, upoll);
   SockListenModule::SLPlug *slmp = slm.makePlug();
   ConAry connections;

   while (!slm.hasError()) {
      EchoConnection::disp.DispatchEvent();

      if (connections.size() > 0) {
	 MaintainList(connections);
      }

      if (slmp->isReadable()) {
	 SocketModuleChunkPtr attemptedchunk = slmp->getConnection();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 cerr << "Got connection from "
	      << const_cast<SocketAddress &>(attempted->GetPeerAddr()) << "\n";
	 attemptedchunk->ReleaseModule();
	 attemptedchunk.ReleasePtr();

	 connections.push_back(new EchoConnection(attempted));
      }
   }
   cerr << '\n' << slm.getError().getErrorString() << '\n';
}

static void MaintainList(ConAry &lst)
{
   for (ConAry::iterator i = lst.begin(); i < lst.end();) {
      EchoConnection *ec = *i;

      if (ec->socket->hasError()) {
//	 cerr << "Error: " << ec->socket->getError().getErrorString() << '\n';
	 lst.erase(i);
	 delete ec;
      } else
	 ++i;
      ec = 0;
   }
}
