#include "StrMod/StreamSplitter.h"
#include "StrMod/StreamFDModule.h"
#include "StrMod/SocketModule.h"
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
#include <EHnet++/InetAddress.h>
#include <iostream.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

extern "C" int atoi(const char *);

int main(int argc, char *argv[])
{
   const char *host = "127.0.0.1";
   int port = 7;  // Echo port

   if (argc > 1)
   {
      host = argv[1];
   }
   if (argc > 2)
   {
      port = atoi(argv[2]);
   }
   InetAddress echoaddr(host, port);

   {
      int temp;

      if ((temp = fcntl(0, F_GETFL, 0)) < 0) {
	 return(1);
      }
      temp &= ~O_NDELAY;
      if (fcntl(0, F_SETFL, temp | O_NONBLOCK) < 0) {
	 return(1);
      }
   }
   {
      int temp;

      if ((temp = fcntl(1, F_GETFL, 0)) < 0) {
	 return(1);
      }
      temp &= ~O_NDELAY;
      if (fcntl(1, F_SETFL, temp | O_NONBLOCK) < 0) {
	 return(1);
      }
   }

   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   StreamFDModule sin(0, pm, StreamFDModule::CheckRead, false);
   StreamFDModule sout(1, pm, StreamFDModule::CheckWrite, false);
   StreamSplitterModule splitter;
   SocketModule sock(echoaddr, pm);

   sin.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideIn))
      );
   sout.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideOut))
      );
   sock.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideBiDir))
      );

   while (!sin.readEOF() || !sout.writeBufferEmpty())
   {
      disp.DispatchEvents(1);
//        cerr << "Tick!\n";
   }
}
