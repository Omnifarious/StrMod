#include "StrMod/StreamSplitter.h"
#include "StrMod/StreamFDModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/FDUtil.h"
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
#include <EHnet++/InetAddress.h>
#include <iostream.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

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
   int error = 0;

   if (FDUtil::setNonBlock(0, error))
   {
      if (!FDUtil::setNonBlock(1, error))
      {
	 cerr << "Couldn't set stdout to non-blocking.\n";
	 return(1);
      }
   }
   else
   {
      cerr << "Couldn't set stdin to non-blocking.\n";
      return(1);
   }

   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   StreamFDModule sin(0, pm, StreamFDModule::CheckRead, false);
   StreamFDModule sout(1, pm, StreamFDModule::CheckWrite, false);
   StreamSplitterModule splitter;
   SocketModule sock(echoaddr, pm);

   sin.setSendChunkOnEOF(true);
   sock.setSendChunkOnEOF(true);
   sin.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideIn))
      );
   sout.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideOut))
      );
   sock.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideBiDir))
      );

   while (!sout.hasError())
   {
      disp.DispatchEvents(1);
//        cerr << "Tick!\n";
   }

   FDUtil::setBlock(0, error);
   FDUtil::setBlock(1, error);
   return(0);
}
