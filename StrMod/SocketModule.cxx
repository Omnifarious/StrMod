/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:48  hopper
// Initial revision
//
// Revision 0.13  1995/04/17  22:56:12  hopper
// Changed things for integration into the rest of my libraries.
//
// Revision 0.12  1995/04/14  17:26:16  hopper
// Combined versions 0.11 and 0.11.0.4
//
// Revision 0.11.0.4  1994/06/16  02:51:51  hopper
// Added #pramga implementation thing.
//
// Revision 0.11.0.3  1994/05/24  04:03:36  hopper
// Fixed bug where I had O_NDELAY instead of ~O_NDELAY.
// Added ability to block, or not block on connect call.
//
// Revision 0.11.0.2  1994/05/17  06:08:23  hopper
// Changed FNDELAY flag to POSIX O_NONBLOCK.
//
// Revision 0.11.0.1  1994/05/16  05:43:35  hopper
// No changes, but I needed a head for the inevitable WinterFire-OS/2
// branch.
//
// Revision 0.11  1994/05/07  03:24:53  hopper
// Changed header files stuff around to be aprroximately right with
// new libraries, and new names & stuff.
//
// Revision 0.10  1992/05/02  01:54:51  hopper
// Genesis!
//

#ifndef NO_RcsID
static char _SocketModule_CC_rcsID[] =
      "$Id$";
#endif

#ifdef __GNUG__
#  pragma implementation "SocketModule.h"
#endif

#include "StrMod/SocketModule.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

extern "C" int close(int);
extern "C" int errno;

const STR_ClassIdent SocketModule::identifier(12UL);

int SocketModule::MakeSocket(SocketModule *obj,
			     const SocketAddress &addr, int blockconnect)
{
   int fd, temp;
   SocketAddress *peer = addr.Copy();

   obj->peer = peer;
   if ((fd = socket(peer->SockAddr()->sa_family, SOCK_STREAM, PF_UNSPEC)) < 0)
      return(fd);

   if (blockconnect) {
      if (connect(fd, (caddr_t)(peer->SockAddr()), peer->AddressSize()) < 0) {
	 if (errno != EINPROGRESS) {
	    close(fd);
	    fd = -1;
	    return(fd);
	 }
      }
   }

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

   if (!blockconnect) {
      if (connect(fd, (caddr_t)(peer->SockAddr()), peer->AddressSize()) < 0) {
	 if (errno != EINPROGRESS) {
	    close(fd);
	    fd = -1;
	    return(fd);
	 }
      }
   }

   return(fd);
}
