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

#include <Dispatch/dispatcher.h>
#include <Dispatch/iohandler.h>
// #include "AudHandle.h"
// #include <ibmpp/String.h>
#include <string>
#include <iostream>

class CoutHandler : public IOHandler {
   Dispatcher *dist;
   int outready;

 public:
   CoutHandler(Dispatcher *d) : dist(d), outready(-1) {}
   virtual ~CoutHandler() {
      if (outready >= 0)
	 dist->unlink(1);
   }

   ostream &str() {
      outready = 1;
      dist->link(1, Dispatcher::WriteMask, this);
      return(cout);
   }

   virtual int outputReady(int fd) {
      if (outready > 0) {
	 cout.flush();
	 outready = 0;
	 return(0);
      } else {
	 outready = -1;
	 return(-1);
      }
   }
};

class CinHandler : public IOHandler {
   int receivedquit;
   CoutHandler *out;

 public:
   CinHandler(CoutHandler *o) : out(o), receivedquit(0) {}

   int ShouldQuit() { return(receivedquit != 0); }

   virtual int inputReady(int fd);
};

class TimerHandler : public IOHandler {
   long sec, usec;
   Dispatcher *dist;
   CoutHandler *out;

 public:
   TimerHandler(long, long, Dispatcher *d, CoutHandler *o);

   virtual void timerExpired(long sec, long usec);
};

TimerHandler::TimerHandler(long sc, long usc, Dispatcher *d, CoutHandler *o) :
              sec(sc), usec(usc), dist(d), out(o)
{
   dist->startTimer(sec, usec, this);
}

CinHandler::inputReady(int fd)
{
   string george;

   if (fd == 0) {
      cin >> george;
      if (george != "quit") {
	 out->str() << "You typed \"" << george << "\".\n";
	 return(0);
      } else {
	 receivedquit = 1;
	 return(-1);
      }
   } else {
      cerr << "Somebody has me handling for fd " << fd << ". I only handle fd 0.\n";
      return(-1);
   }
}

void TimerHandler::timerExpired(long sc, long usc)
{
   dist->startTimer(sec, usec, this);
   out->str() << "The timer has expired.\n";
   out->str() << "Setting again for " << sec << "s + " << usec << "us.\n";
}

int main(int argc, char *argv[])
{
   Dispatcher *dist = &(Dispatcher::instance());
   CoutHandler h2(dist);;

   h2.str() << "Started.\n";

   CinHandler handle(&h2);
//   AudioHandler ahan(dist, AudioHandler::Input);
   TimerHandler thdl(5, 0, dist, &h2);

   dist->link(0, Dispatcher::ReadMask, &handle);
   while (!handle.ShouldQuit())
      dist->dispatch();
}
