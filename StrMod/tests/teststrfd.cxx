#include "StrMod/StreamFDModule.h"
#include "StrMod/EchoModule.h"
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
// #include <EHnet++/InetAddress.h>
#include <iostream.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

// $Revision$
// For change log see ../ChangeLog

int main(int argc, char *argv[])
{
   // InetAddress echoaddr(argv[1], 7);
   // StreamFDModule StdIn(0, StreamFDModule::CheckRead, false);
   // StreamFDModule StdOut(1, StreamFDModule::CheckWrite, false);
   int fd = open("/dev/tty", O_RDWR);
   if (fd >= 0)
   {
      int temp;

      if ((temp = fcntl(fd, F_GETFL, 0)) < 0) {
	 close(fd);
	 fd = -1;
	 return(fd);
      }
      temp &= ~O_NDELAY;
      if (fcntl(fd, F_SETFL, temp | O_NONBLOCK) < 0) {
	 close(fd);
	 fd = -1;
	 return(fd);
      }
   }
   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   StreamFDModule fdmod(fd, pm);
   EchoModule echo;

   fdmod.makePlug(0)->plugInto(*(echo.makePlug(0)));

   while (!fdmod.readEOF())
   {
      disp.DispatchEvents(1);
   }
}
