#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include "StrMod/SimpleMulti.h"
#include "StrMod/ProcessorModule.h"
#include "StrMod/NewlineChopper.h"
#include "StrMod/PassThrough.h"
#include <EHnet++/InetAddress.h>
#include <Dispatch/dispatcher.h>
#include <vector>
#include <iostream.h>

extern "C" int atoi(const char *);

class MyProcessor : public ProcessorModule {
 public:
   enum Sides { ToNet, ToMulti };

   inline MyProcessor();

   inline bool CanCreate(Sides side) const;
   inline Plug *MakePlug(Sides side);

 private:
   NewlineChopper chopper_;
   PassThrough passthrough_;
};

MyProcessor::MyProcessor() : ProcessorModule(&chopper_, &passthrough_)
{
}

inline bool MyProcessor::CanCreate(Sides side) const
{
   return(ProcessorModule::CanCreate((side == ToNet) ? OneSide : OtherSide));
}

inline MyProcessor::Plug *MyProcessor::MakePlug(Sides side)
{
   return(ProcessorModule::MakePlug((side == ToNet) ? OneSide : OtherSide));
}

struct EchoConnection {
   static SimpleMultiplexer *multip;

   SocketModule *socket;
   MyProcessor proc;
   StrPlug *multiplug;

   inline SimpleMultiplexer *multiplexer();

   inline EchoConnection(SocketModule *sock);
   inline virtual ~EchoConnection();

 private:
   void makeMultiplexer();
};

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
   StrPlug *p1 = socket->MakePlug(0);
   StrPlug *p2 = proc.MakePlug(MyProcessor::ToNet);

   p1->PlugIntoAndNotify(p2);

   cerr << "Here 3!\n";
   p1 = proc.MakePlug(MyProcessor::ToMulti);
   p2 = multiplexer()->MakePlug(SimpleMultiplexer::MultiSide);

   p1->PlugIntoAndNotify(p2);
   multiplug = p2;
   cerr << "Here 4!\n";
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
   multiplug->ModuleFrom()->DeletePlug(multiplug);
}

void EchoConnection::makeMultiplexer()
{
   EchoModule *echom = new EchoModule();
   multip = new SimpleMultiplexer();
   StrPlug *p1 = echom->MakePlug();
   StrPlug *p2 = multip->MakePlug(SimpleMultiplexer::SingleSide);

   p1->PlugIntoAndNotify(p2);
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

   InetAddress      here(atoi(argv[1]));
   SockListenModule slm(here);
   ListeningPlug   *slmp = slm.MakePlug();
   ConAry           connections;

   while (!slm.HasError()) {
      Dispatcher::instance().dispatch();

      if (connections.size() > 0) {
	 MaintainList(connections);
      }

      if (slmp->CanRead()) {
	 SocketModuleChunkPtr attemptedchunk = slmp->Read();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 cerr << "Got connection from " << *(attempted->GetPeerAddr()) << "\n";
	 attemptedchunk->ReleaseModule();
	 attemptedchunk.ReleasePtr();

	 connections.push_back(new EchoConnection(attempted));
      }
   }
   cerr << '\n' << slm.ErrorString() << '\n';
}

static void MaintainList(ConAry &lst)
{
   for (ConAry::iterator i = lst.begin(); i < lst.end();) {
      EchoConnection *ec = *i;

      if (ec->socket->HasError()) {
	 cerr << "Error: " << ec->socket->ErrorString() << '\n';
	 lst.erase(i);
	 delete ec;
      } else
	 ++i;
      ec = 0;
   }
}
