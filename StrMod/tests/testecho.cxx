#include "StrMod/SocketModule.h"
#include "StrMod/StreamFDModule.h"
#include "StrMod/StreamSplitter.h"
#include <Dispatch/dispatcher.h>
#include <EHnet++/InetAddress.h>
#include <iostream.h>
#include <assert.h>

int main(int argc, char *argv[])
{
   InetAddress echoaddr(argv[1], 7);
   StreamFDModule StdIn(0, StreamFDModule::CheckRead, false);
   StreamFDModule StdOut(1, StreamFDModule::CheckWrite, false);
   SocketModule echo(echoaddr);
   SplitterModule splitter;
   StrPlug *t1, *t2;

   t1 = splitter.MakePlug(SplitterModule::CombinedPlug);
   t2 = echo.MakePlug(0);
   assert(t1 && t2);

   t1->PlugIntoAndNotify(t2);

   t1 = splitter.MakePlug(SplitterModule::OutputOnlyPlug);
   t2 = StdOut.MakePlug(0);
   assert(t1 && t2);

   t1->PlugInto(t2);
   t1->NotifyOnReadable();
   t2->NotifyOnWriteable();

   t1 = splitter.MakePlug(SplitterModule::InputOnlyPlug);
   t2 = StdIn.MakePlug(0);
   assert(t1 && t2);

   t1->PlugInto(t2);
   t1->NotifyOnWriteable();
   t2->NotifyOnReadable();

   while (!StdIn.HasError() && !StdOut.HasError())
      Dispatcher::instance().dispatch();
   if (StdIn.HasError())
      cerr << "Error on StdIn: " << StdIn.ErrorString() << '\n';
   if (StdOut.HasError())
      cerr << "Error on StdOut: " << StdOut.ErrorString() << '\n';
}
