#include "StrMod/StreamSplitter.h"
#include "StrMod/StreamFDModule.h"
#include "StrMod/EchoModule.h"
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
#include <iostream.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

// $Revision$
// For change log see ../ChangeLog

int main(int argc, char *argv[])
{
   // InetAddress echoaddr(argv[1], 7);
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
   EchoModule echo;

   sin.setSendChunkOnEOF(true);
   sin.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideIn))
      );
   sout.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideOut))
      );
   echo.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideBiDir))
      );

   while (!sout.hasError())
   {
      disp.DispatchEvents(1);
//        cerr << "Tick!\n";
   }
}
