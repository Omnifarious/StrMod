#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/StreamFDModule.h"
#include <EHnet++/InetAddress.h>
#include <Dispatch/dispatcher.h>
#include <iostream.h>
#include <string>

extern "C" int atoi(const char *);

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <port #>\n";
      return(1);
   }

   InetAddress      here(atoi(argv[1]));
   SockListenModule slm(here);
   StreamFDModule   output(1, StreamFDModule::CheckWrite, true);
   ListeningPlug   *slmp = slm.MakePlug();
   StrPlug         *outplug = output.MakePlug(0);
   SocketModule    *cursock = 0;

   while (!slm.HasError()) {
      Dispatcher::instance().dispatch();
      if (cursock && cursock->HasError()) {
	 cerr << "\nConnection to: " << *(cursock->GetPeerAddr())
	      << " closed\n";
	 cerr << cursock->ErrorString() << '\n';
	 delete cursock;
	 cursock = 0;
      }
      if (slmp->CanRead()) {
	 SocketModuleChunk *attemptedchunk = slmp->Read();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 attemptedchunk->ReleaseModule();
	 delete attemptedchunk;
	 cerr << "\nGot connection from: " << *(attempted->GetPeerAddr())
	      << "\n";
	 if (cursock != 0) {
	    cerr << "But there already is a currently active connection, so ";
	    cerr << "this one is shut down.\n";
	    delete attempted;
	 } else {
	    cursock = attempted;

	    cout << "cursock->MaxBlockSize() == " << cursock->GetMaxBlockSize()
	         << '\n';
	    cout << "output.MaxBlockSize() == " << output.GetMaxBlockSize()
	         << endl;
	    StrPlug *csplug = cursock->MakePlug(0);

	    csplug->PlugInto(outplug);
	    csplug->NotifyOnReadable();
	    outplug->NotifyOnWriteable();
	 }
      }
   }
   cerr << '\n' << slm.ErrorString() << '\n';
}
