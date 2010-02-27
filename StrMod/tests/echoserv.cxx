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
#include <EHnet++/InetAddress.h>
#include <UniEvent/UnixEventPoll.h>
#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/RegistryDispatcherGlue.h>
#include <iostream>

using std::cerr;

extern "C" int atoi(const char *);

using ::strmod::strmod::SocketModule;
using ::strmod::strmod::StreamModule;
using ::strmod::strmod::StreamFDModule;
using ::strmod::strmod::EchoModule;

class EchoConnection;

class ConnectionError : public strmod::unievent::Event
{
 public:
   ConnectionError(EchoConnection *econ) : econ_(econ)
   {
   }
   virtual ~ConnectionError();

   virtual void triggerEvent(::strmod::unievent::Dispatcher *disp = 0);

 private:
   EchoConnection *econ_;
};

struct EchoConnection
{
   SocketModule *socket;
   EchoModule echo;
   ::strmod::unievent::EventPtr conerr_;

   inline EchoConnection(SocketModule *sock);
   inline virtual ~EchoConnection();
};

inline EchoConnection::EchoConnection(SocketModule *sock)
     : socket(sock), conerr_(new ConnectionError(this))
{
   sock->setSendChunkOnEOF(true);

   StreamModule::Plug *p1 = socket->makePlug(0);
   StreamModule::Plug *p2 = echo.makePlug(0);

   p1->plugInto(*p2);
   sock->onErrorIn(StreamFDModule::ErrRead, conerr_);
   sock->onErrorIn(StreamFDModule::ErrWrite, conerr_);
   sock->onErrorIn(StreamFDModule::ErrGeneral, conerr_);
   sock->onErrorIn(StreamFDModule::ErrFatal, conerr_);
}

inline EchoConnection::~EchoConnection()
{
   cerr << "Farwell cruel world!\n";
   delete socket;
}

void ConnectionError::triggerEvent(::strmod::unievent::Dispatcher *disp)
{
   ::std::cerr << "The socket had an error.\n";
   delete econ_;
   econ_ = 0;
}

ConnectionError::~ConnectionError()
{
   ::std::cerr << "ConnectionError going away!\n";
}

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <port #>\n";
      return(1);
   }

   ::strmod::unievent::SimpleDispatcher disp;
   ::strmod::unievent::UnixEventPoll upoll(&disp);
   ::strmod::unievent::RegistryDispatcherGlue glue(&disp, &upoll);
   InetAddress      here(atoi(argv[1]));
   ::strmod::strmod::SockListenModule slm(here, disp, upoll, 8);
   ::strmod::strmod::SockListenModule::SLPlug *slmp = slm.makePlug();

   while (!slm.hasError())
   {
      disp.dispatchEvents(1);
      if (slmp->isReadable())
      {
         using ::strmod::strmod::SocketModuleChunkPtr;
	 SocketModuleChunkPtr attemptedchunk = slmp->getConnection();
	 SocketModule *attempted = attemptedchunk->getModule();

	 cerr << "Got connection from "
	      << const_cast<SocketAddress &>(attempted->GetPeerAddr())
	      << "\n";
         new EchoConnection(attempted);
      }
   }
   {
      char buf[256];
      slm.getError().getErrorString(buf, sizeof(buf) - 1);
      cerr << '\n' << buf << '\n';
   }
}
