#include <Dispatch/dispatcher.h>
#include <Dispatch/iohandler.h>
// #include "AudHandle.h"
#include <ibmpp/String.h>
#include <iostream.h>

char String::junk = 0;

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

const String test("quit", 1);

CinHandler::inputReady(int fd)
{
   String george;

   if (fd == 0) {
      cin >> george;
      if (george != test) {
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
