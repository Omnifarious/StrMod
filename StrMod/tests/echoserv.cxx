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

#include "StrMod/SockListenModule.h"
#include "StrMod/SocketModule.h"
#include "StrMod/EchoModule.h"
#include <EHnet++/InetAddress.h>
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
#include <vector>
#include <iostream>

using std::cerr;

extern "C" int atoi(const char *);

struct EchoConnection {
   SocketModule *socket;
   EchoModule echo;

   inline EchoConnection(SocketModule *sock);
   inline virtual ~EchoConnection();
};

inline EchoConnection::EchoConnection(SocketModule *sock) : socket(sock)
{
   sock->setSendChunkOnEOF(true);

   StreamModule::Plug *p1 = socket->makePlug(0);
   StreamModule::Plug *p2 = echo.makePlug(0);

   p1->plugInto(*p2);
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
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

   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   InetAddress      here(atoi(argv[1]));
   SockListenModule slm(here, pm, 8);
   SockListenModule::SLPlug *slmp = slm.makePlug();
   ConAry           connections;

   while (!slm.hasError()) {
      disp.DispatchEvents(1);
      // cerr << "Tick!\n";
      if (connections.size() > 0) {
	 MaintainList(connections);
      }

      if (slmp->isReadable()) {
	 SocketModuleChunkPtr attemptedchunk = slmp->getConnection();
	 SocketModule *attempted = attemptedchunk->GetModule();

	 cerr << "Got connection from "
	      << const_cast<SocketAddress &>(attempted->GetPeerAddr())
	      << "\n";
	 attemptedchunk->ReleaseModule();
	 attemptedchunk.ReleasePtr();

	 connections.push_back(new EchoConnection(attempted));
      }
   }
   cerr << '\n' << slm.getError().getErrorString() << '\n';
}

static void MaintainList(ConAry &lst)
{
   for (ConAry::iterator i = lst.begin(); i != lst.end();)
   {
      EchoConnection *ec = *i;

      if (ec->socket->readEOF() || ec->socket->hasError())
      {
	 if (ec->socket->readEOF())
	 {
	    cerr << "Sucking an empty pipe.\n";
	 }
	 else
	 {
	    cerr << "The socket had an error.\n";
	 }
	 lst.erase(i);
	 delete ec;
	 ec = 0;
      }
      else
      {
	 ++i;
      }
   }
}
