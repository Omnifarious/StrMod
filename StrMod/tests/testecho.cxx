/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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
