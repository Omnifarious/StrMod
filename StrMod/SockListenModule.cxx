/* $Header$ */

#ifndef NO_RcsID
static char _SockListenModule_CC_rcsID[] =
      "$Id$";
#endif

#ifdef __GNUG__
#  pragma implementation "SockListenModule.h"
#endif

#include "StrMod/SockListenModule.h"

#ifndef _STR_SocketModule_H_
#  include "StrMod/SocketModule.h"
#endif

#include <EHnet++/SocketAddress.h>
#include <EHnet++/InetAddress.h>
#include <Dispatch/dispatcher.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <rw/cstring.h>
#include <iostream.h>
#include <unistd.h>

const STR_ClassIdent SockListenModule::identifier(13UL);
const STR_ClassIdent ListeningPlug::identifier(14UL);
const STR_ClassIdent SocketModuleChunk::identifier(15UL);

RWCString SockListenModule::ErrorString() const
{
   int errn = ErrorNum();

   if (errn == 0)
      return("no error at all, nothing happened, nope :-)");
   else if (errn > 0) {
      char *err = strerror(errn);

      if (err)
	 return(err);
      else
	 return("unkown error");
   } else if (errn == -1)
      return("end of file encountered");
   else
      return("unknown error");
}

SockListenModule::SockListenModule(const SocketAddress &bind_addr, int qlen) :
      sockfd(-1), last_error(0), lplug(0), plug_pulled(false), newmodule(0),
      myaddr(0)
{
   int temp;

   myaddr = bind_addr.Copy();
   if ((sockfd =
	   socket(myaddr->SockAddr()->sa_family, SOCK_STREAM, PF_UNSPEC)) < 0) {
      last_error = errno;
      return;
   }
   if ((temp = fcntl(sockfd, F_GETFL, 0)) < 0) {
      last_error = errno;
      close(sockfd);
      sockfd = -1;
      return;
   }
   temp &= O_NDELAY;
   if (fcntl(sockfd, F_SETFL, temp | O_NONBLOCK) < 0) {
      last_error = errno;
      close(sockfd);
      sockfd = -1;
      return;
   }
   if (bind(sockfd, myaddr->SockAddr(), myaddr->AddressSize()) < 0) {
      last_error = errno;
      close(sockfd);
      sockfd = -1;
      return;
   }
   if (listen(sockfd, qlen) < 0) {
      last_error = errno;
      close(sockfd);
      sockfd = -1;
      return;
   }
   lplug = new ListeningPlug(this);
}

SockListenModule::~SockListenModule()
{
   delete lplug;
   if (sockfd >= 0)
      close(sockfd);
}

StrChunk *ListeningPlug::InternalRead()
{
   SockListenModule *parent = ModuleFrom();
   StrChunk *temp;

   if (!CanRead()) {
      Dispatcher tempd;

      tempd.link(parent->sockfd, Dispatcher::ReadMask, this);
      while (parent->sockfd >= 0 && parent->last_error == 0 && !CanRead())
	 tempd.dispatch();
      if (!CanRead())
	 return(0);
   }
   temp = new SocketModuleChunk(parent->newmodule);
   parent->newmodule = 0;
   Dispatcher::instance().link(parent->sockfd, Dispatcher::ReadMask, this);
   return(temp);
}

void ListeningPlug::WriteableNotify()
{
   if (PluggedInto() && CanRead())
      PluggedInto()->Write(Read());
}

int ListeningPlug::inputReady(int fd)
{
   SockListenModule *parent = ModuleFrom();
   int newfd;
   unsigned int length;
   static unsigned char buf[1024];
   sockaddr *saddr;

   if (fd != parent->sockfd || parent->newmodule != 0 ||
       parent->last_error != 0) {
      Dispatcher::instance().unlink(fd, Dispatcher::ReadMask);
      return(0);
   }

   length = 1024;
   saddr = static_cast<sockaddr *>(buf);
   if ((newfd = accept(parent->sockfd, saddr, &length)) < 0) {
      parent->last_error = errno;
      return(0);
   }
   if (saddr->sa_family != AF_INET) {
      cerr << "Aigh! ListeningPlug::inputReady(int) accepted a connection ";
      cerr << "from a non-Internet\naddress. I don't know how to handle ";
      cerr << "that.\n";
      close(newfd);
      return(0);
   } else {
      SocketAddress *peer = new InetAddress(*((sockaddr_in *)buf));
      int temp;

      if ((temp = fcntl(newfd, F_GETFL, 0)) < 0) {
	 parent->last_error = errno;
	 close(newfd);
	 return(0);
      }
      temp &= ~O_NDELAY;
      if (fcntl(newfd, F_SETFL, temp | O_NONBLOCK) < 0) {
	 parent->last_error = errno;
	 close(newfd);
	 return(0);
      }
      parent->newmodule = parent->MakeSocketModule(newfd, peer);
      DoReadableNotify();
      return(0);
   }
}

bool ListeningPlug::Write(StrChunk *w)
{
   delete w;
   return(true);
}

ListeningPlug::ListeningPlug(SockListenModule *p) : parent(p)
{
   if (p->sockfd >= 0)
      Dispatcher::instance().link(p->sockfd, Dispatcher::ReadMask, this);
}

ListeningPlug::~ListeningPlug()
{
   SockListenModule *parent = ModuleFrom();

   if (parent->newmodule)
      delete parent->newmodule;
   Dispatcher::instance().unlink(parent->sockfd);
}

int SocketModuleChunk::junk = 0;

// Started using cvs, and ChangeLog.  Look there for a revision history.
//
// Revision 0.13  1995/04/18  03:36:46  hopper
// Merged revisions 0.12 and 0.12.0.3
//
// Revision 0.12.0.3  1994/07/18  03:44:00  hopper
// Added #pragma implementation thing so this would work better with gcc 2.6.0
//
// Revision 0.12.0.2  1994/05/17  06:07:19  hopper
// Added delcaration for strerror, since g++ seems to want it...
// Also changed FNDELAY flag to POSIX O_NONBLOCK.
//
// Revision 0.12.0.1  1994/05/17  05:22:30  hopper
// Changed to use Rogue Wave's RWCString, instead of my String.
//
// Revision 0.12  1994/05/17  05:20:16  hopper
// Changed to use ANSI error string lookup routines.
// Also fixed one include of old header file.
//
// Revision 0.11  1994/05/07  03:24:53  hopper
// Changed header files stuff around to be aprroximately right with
// new libraries, and new names & stuff.
//
// Revision 0.10  1992/05/17  23:30:54  hopper
// Genesis!
//
