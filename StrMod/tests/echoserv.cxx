#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include <EHnet++/InetAddress.h>
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
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
   StreamModule::Plug *p1 = socket->makePlug(0);
   StreamModule::Plug *p2 = echo.makePlug(0);

   p1->plugInto(*p2);
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

   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   InetAddress      here(atoi(argv[1]));
   SockListenModule slm(here, pm, 8);
   SockListenModule::SLPlug *slmp = slm.makePlug();
   ConAry           connections;

   while (!slm.hasError()) {
      disp.DispatchEvents(1);
      if (connections.size() > 0) {
	 MaintainList(connections);
      }

      if (slmp->isReadable()) {
	 SocketModuleChunkPtr attemptedchunk = slmp->getConnection();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 cerr << "Got connection from " << attempted->GetPeerAddr() << "\n";
	 attemptedchunk->ReleaseModule();
	 attemptedchunk.ReleasePtr();

	 connections.push_back(new EchoConnection(attempted));
      }
   }
   cerr << '\n' << slm.getError().getErrorString() << '\n';
}

static void MaintainList(ConAry &lst)
{
   for (ConAry::iterator i = lst.begin(); i != lst.end();)
   {
      EchoConnection *ec = *i;

      if (ec->socket->readEOF() || ec->socket->hasError())
      {
	 if (ec->socket->readEOF())
	 {
	    cerr << "Sucking an empty pipe.\n";
	 }
	 else
	 {
	    cerr << "The socket had an error.\n";
	 }
	 lst.erase(i);
	 delete ec;
	 ec = 0;
      }
      else
      {
	 ++i;
      }
   }
}
