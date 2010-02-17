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
