/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

/* $Header$ */

// For a list of changes, see ../ChangeLog

#include "TelnetToAscii.h"
#include <StrMod/TelnetChunker.h>
#include <StrMod/SimpleTelnetClient.h>
#include <StrMod/SimpleMulti.h>
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
      if (FDUtil::setNonBlock(1, error))
      {
	 if (!FDUtil::setNonBlock(2, error))
	 {
	    cerr << "Couldn't set stderr to non-blocking.\n";
	    return(1);
	 }
      }
      else
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
   StreamFDModule serr(2, pm, StreamFDModule::CheckWrite, false);
   StreamSplitterModule splitter;
   SimpleTelnetClient tc;
   ProcessorModule prmdlocal(*(new TelnetToAscii("LOCAL", true)),
			     *(new PassThrough));
   ProcessorModule prmdrmt(*(new TelnetToAscii("REMOTE", true)),
			   *(new PassThrough));
   SimpleMultiplexer splitlocal(disp);
   SimpleMultiplexer splitremote(disp);
   SimpleMultiplexer combineout(disp);
   ProcessorModule parser(*(new PassThrough), *(new TelnetChunker));
   SocketModule sock(echoaddr, pm);

   sin.setSendChunkOnEOF(true);
   sock.setSendChunkOnEOF(true);
   sin.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideIn))
      );
   sout.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideOut))
      );
   tc.makePlug(SimpleTelnetClient::ToUser)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideBiDir))
      );
   splitlocal.makePlug(SimpleMultiplexer::MultiSide)->plugInto(
      *(splitremote.makePlug(SimpleMultiplexer::MultiSide))
      );
   prmdlocal.makePlug(ProcessorModule::OneSide)->plugInto(
      *(splitlocal.makePlug(SimpleMultiplexer::MultiSide))
      );
   prmdrmt.makePlug(ProcessorModule::OneSide)->plugInto(
      *(splitremote.makePlug(SimpleMultiplexer::MultiSide))
      );
   prmdlocal.makePlug(ProcessorModule::OtherSide)->plugInto(
      *(combineout.makePlug(SimpleMultiplexer::MultiSide))
      );
   prmdrmt.makePlug(ProcessorModule::OtherSide)->plugInto(
      *(combineout.makePlug(SimpleMultiplexer::MultiSide))
      );
   combineout.makePlug(SimpleMultiplexer::SingleSide)->plugInto(
      *(serr.makePlug(0))
      );
   parser.makePlug(ProcessorModule::OneSide)->plugInto(
      *(splitremote.makePlug(SimpleMultiplexer::SingleSide))
      );
   sock.makePlug(0)->plugInto(
      *(parser.makePlug(ProcessorModule::OtherSide))
      );
   tc.makePlug(SimpleTelnetClient::ToServer)->plugInto(
      *(splitlocal.makePlug(SimpleMultiplexer::SingleSide))
      );

   while (!sout.hasError())
   {
      disp.dispatchEvents(1);
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
