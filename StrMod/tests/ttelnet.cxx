/* $Header$ */

// For a list of changes, see ../ChangeLog

#include "TelnetToAscii.h"
#include <StrMod/TelnetParser.h>
#include <StrMod/PassThrough.h>
#include <StrMod/ProcessorModule.h>
#include <StrMod/SocketModule.h>
#include <StrMod/StreamSplitter.h>
#include <StrMod/StreamFDModule.h>
#include <StrMod/SocketModule.h>
#include <StrMod/FDUtil.h>
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
#include <EHnet++/InetAddress.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

static void handle_intr(int);

int main(int argc, char *argv[])
{
   signal(SIGINT, handle_intr);
   const char *host = "127.0.0.1";
   int port = 23;  // Telnet port

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
   ProcessorModule prmd1(*(new TelnetParser), *(new PassThrough));
   ProcessorModule prmd2(*(new TelnetToAscii("REMOTE")),
			 *(new TelnetToAscii("LOCAL")));
   SocketModule sock(echoaddr, pm);

   sin.setSendChunkOnEOF(true);
   sock.setSendChunkOnEOF(true);
   sin.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideIn))
      );
   sout.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideOut))
      );
   prmd2.makePlug(ProcessorModule::OtherSide)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideBiDir))
      );
   prmd1.makePlug(ProcessorModule::OtherSide)->plugInto(
      *(prmd2.makePlug(ProcessorModule::OneSide))
      );
   sock.makePlug(0)->plugInto(
      *(prmd1.makePlug(ProcessorModule::OneSide))
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

static void handle_intr(int sig)
{
   int error = 0;

   FDUtil::setBlock(0, error);
   FDUtil::setBlock(1, error);
   signal(sig, SIG_DFL);
   kill(getpid(), sig);
}