#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include <EHnet++/InetAddress.h>
#include <Dispatch/dispatcher.h>
#include <vector>
#include <iostream.h>

extern "C" int atoi(const char *);

struct EchoConnection {
   SocketModule *socket;
   EchoModule echo;

   inline EchoConnection(SocketModule *sock);
   inline virtual ~EchoConnection();
};

inline EchoConnection::EchoConnection(SocketModule *sock) : socket(sock)
{
   StrPlug *p1 = socket->MakePlug(0);
   StrPlug *p2 = echo.MakePlug();

   p1->PlugIntoAndNotify(p2);
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
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
	 SocketModuleChunk *attemptedchunk = slmp->Read();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 attemptedchunk->ReleaseModule();
	 delete attemptedchunk;

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
