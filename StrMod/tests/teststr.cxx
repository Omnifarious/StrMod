#include <StrMod/SocketModule.h>
#include <EHnet++/InetAddress.h>
#include <StrMod/StreamFDModule.h>
#include <StrMod/StreamSplitter.h>
#include <Dispatch/dispatcher.h>
#include <iostream.h>
#include <unistd.h>
#include <fcntl.h>
#include <ibmpp/String.h>

extern "C" exit(int);
extern "C" void perror(const char *);
extern "C" int atoi(const char *);

int main(int argc, char *argv[])
{
   int fd1, fd2;

   if (argc != 4) {
      cerr << "Usage: " << argv[0];
      cerr << " <input file> <destination machine> <port>\n";
      exit(1);
   }
   if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
      cerr << "Error opening \"" << argv[1] << "\"\n";
      perror(argv[0]);
      exit(2);
   }

   InetAddress sockaddr(argv[2], atoi(argv[3]));
   SocketModule FDsocket(sockaddr);

   fcntl(fd1, F_SETFL, fcntl(fd1, F_GETFL, 0) | FNDELAY);

   StreamFDModule FDinput(fd1, 1, 0), FDoutput(1, 0, 1);
   SplitterModule tee;
   StrPlug *pfdin = FDinput.MakePlug(0), *pfdsock = FDsocket.MakePlug(0),
           *pfdout = FDoutput.MakePlug(0);
   StrPlug *pinput   =  tee.MakePlug(SplitterModule::InputOnlyPlug),
           *poutput  =  tee.MakePlug(SplitterModule::OutputOnlyPlug),
           *pcombined = tee.MakePlug(SplitterModule::CombinedPlug);

   pfdin->PlugInto(pinput);
   pfdout->PlugInto(poutput);
   pfdsock->PlugInto(pcombined);
   pfdin->NotifyOnReadable();
   pfdout->NotifyOnWriteable();
   pfdsock->NotifyOnReadable();
   pfdsock->NotifyOnWriteable();
   pinput->NotifyOnWriteable();
   poutput->NotifyOnReadable();
   pcombined->NotifyOnReadable();
   pcombined->NotifyOnWriteable();
   FDsocket.SetMaxBlockSize(1024);
   while (!FDsocket.HasError() && !FDinput.HasError())
      Dispatcher::instance().dispatch();
   if (FDsocket.HasError())
      cout << "FDsocket.ErrorString() == " << FDsocket.ErrorString() << endl;
   if (FDinput.HasError())
      cout << "FDinput.ErrorString() == " << FDinput.ErrorString() << endl;
}
