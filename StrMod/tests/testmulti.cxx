#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include "StrMod/SimpleMulti.h"
#include <EHnet++/InetAddress.h>
#include <Dispatch/dispatcher.h>
#include <vector>
#include <iostream.h>

extern "C" int atoi(const char *);

struct EchoConnection {
   static SimpleMultiplexer *multip;

   SocketModule *socket;

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
   cerr << "Here 2!\n";
   StrPlug *p2 = multiplexer()->MakePlug(SimpleMultiplexer::MultiSide);
   cerr << "Here 3!\n";

   p1->PlugIntoAndNotify(p2);
   cerr << "Here 4!\n";
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
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
