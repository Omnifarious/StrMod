#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include <EHnet++/InetAddress.h>
#include <Dispatch/dispatcher.h>
#include <iostream.h>
#include <ibmpp/String.h>
#include <ibmpp/DL_List.h>
#define RWNO_STD_TYPEDEFS
#include <rw/cstring.h>

extern "C" int atoi(const char *);

char String::junk = 0;

struct EchoConnection : public Object {
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

static void MaintainList(DL_List &lst);

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <port #>\n";
      return(1);
   }

   InetAddress      here(atoi(argv[1]));
   SockListenModule slm(here);
   ListeningPlug   *slmp = slm.MakePlug();
   DL_List          connections;

   while (!slm.HasError()) {
      Dispatcher::instance().dispatch();

      if (connections.HowMany() > 0)
	 MaintainList(connections);

      if (slmp->CanRead()) {
	 SocketModuleChunk *attemptedchunk = slmp->Read();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 attemptedchunk->ReleaseModule();
	 delete attemptedchunk;

	 cerr << "Here!\n";
	 connections.Add(new EchoConnection(attempted));
      }
   }
   cerr << '\n' << slm.ErrorString() << '\n';
}

static void MaintainList(DL_List &lst)
{
   DLL_Cursor i(lst);

   for (i.RestartAtFront(); i.IsValid();) {
      EchoConnection *ec = static_cast<EchoConnection *>(i.cur());

      if (ec->socket->HasError()) {
	 cerr << "Error: " << ec->socket->ErrorString() << '\n';
	 i.DeleteHere(DLL_Cursor::MoveForward);
      } else
	 ++i;
      ec = 0;
   }
}
