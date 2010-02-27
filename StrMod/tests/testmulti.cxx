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

#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include "StrMod/SimpleMulti.h"
#include "StrMod/ProcessorModule.h"
#include "StrMod/NewlineChopper.h"
#include "StrMod/PassThrough.h"
#include <EHnet++/InetAddress.h>
#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/UNIXpollManagerImp.h>
#include <vector>
#include <iostream>

extern "C" int atoi(const char *);

class MyProcessor : public ProcessorModule {
 public:
   enum Sides { ToNet = OneSide, ToMulti = OtherSide};

   inline MyProcessor();

 private:
   NewlineChopper chopper_;
   PassThrough passthrough_;
};

MyProcessor::MyProcessor() : ProcessorModule(chopper_, passthrough_, false)
{
}

struct EchoConnection {
   static SimpleMultiplexer *multip;
   static UNISimpleDispatcher disp;

   SocketModule *socket;
   MyProcessor proc;
   StreamModule::Plug *multiplug;

   inline SimpleMultiplexer *multiplexer();

   inline EchoConnection(SocketModule *sock);
   inline virtual ~EchoConnection();

 private:
   void makeMultiplexer();
};

UNISimpleDispatcher EchoConnection::disp;
SimpleMultiplexer *EchoConnection::multip = 0;

inline SimpleMultiplexer *EchoConnection::multiplexer()
{
   if (multip == 0) {
      makeMultiplexer();
   }
   return(multip);
}

inline EchoConnection::EchoConnection(SocketModule *sock) : socket(sock)
{
   StreamModule::Plug *p1 = socket->makePlug(0);
   StreamModule::Plug *p2
      = proc.makePlug(MyProcessor::ToNet);
   p1->plugInto(*p2);

   p1 = proc.makePlug(MyProcessor::ToMulti);
   p2 = multiplexer()->makePlug(SimpleMultiplexer::MultiSide);
   p1->plugInto(*p2);

   multiplug = p2;
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
   multiplug->getParent().deletePlug(multiplug);
}

void EchoConnection::makeMultiplexer()
{
   EchoModule *echom = new EchoModule();
   multip = new SimpleMultiplexer(disp);
   StreamModule::Plug *p1 = echom->makePlug();
   StreamModule::Plug *p2 = multip->makePlug(SimpleMultiplexer::SingleSide);

   p1->plugInto(*p2);
}

template class vector<EchoConnection *>;
typedef vector<EchoConnection *> ConAry;

static void MaintainList(ConAry &lst);

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <port #>\n";
      return(1);
   }

   UNIXpollManagerImp upoll(&EchoConnection::disp);
   InetAddress here(atoi(argv[1]));
   SockListenModule slm(here, upoll);
   SockListenModule::SLPlug *slmp = slm.makePlug();
   ConAry connections;

   while (!slm.hasError()) {
      EchoConnection::disp.DispatchEvent();

      if (connections.size() > 0) {
	 MaintainList(connections);
      }

      if (slmp->isReadable()) {
	 SocketModuleChunkPtr attemptedchunk = slmp->getConnection();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 cerr << "Got connection from "
	      << const_cast<SocketAddress &>(attempted->GetPeerAddr()) << "\n";
	 attemptedchunk->ReleaseModule();
	 attemptedchunk.ReleasePtr();

	 connections.push_back(new EchoConnection(attempted));
      }
   }
   cerr << '\n' << slm.getError().getErrorString() << '\n';
}

static void MaintainList(ConAry &lst)
{
   for (ConAry::iterator i = lst.begin(); i < lst.end();) {
      EchoConnection *ec = *i;

      if (ec->socket->hasError()) {
//	 cerr << "Error: " << ec->socket->getError().getErrorString() << '\n';
	 lst.erase(i);
	 delete ec;
      } else
	 ++i;
      ec = 0;
   }
}
