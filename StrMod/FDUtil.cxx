/* $Header$ */

// For a log, see ./ChangeLog

// $Revision$

#ifdef __GNUG__
#  pragma implementation "FDUtil.h"
#endif

#include "StrMod/FDUtil.h"
#include <errno.h>
#include <fcntl.h>

bool FDUtil::setNonBlock(int fd, int &myerrno)
{
   int temp;

   if ((temp = fcntl(fd, F_GETFL, 0)) < 0) {
      myerrno = errno;
      return(false);
   }
   temp &= ~O_NDELAY;
   if (fcntl(fd, F_SETFL, temp | O_NONBLOCK) < 0) {
      myerrno = errno;
      return(false);
   }
   return(true);
}

bool FDUtil::setBlock(int fd, int &myerrno)
{
   int temp;

   if ((temp = fcntl(fd, F_GETFL, 0)) < 0) {
      myerrno = errno;
      return(false);
   }
   temp &= ~(O_NDELAY | O_NONBLOCK);
   if (fcntl(fd, F_SETFL, temp) < 0) {
      myerrno = errno;
      return(false);
   }
   return(true);
}
