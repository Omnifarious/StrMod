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

#include "StrMod/StreamFDModule.h"
#include "StrMod/EchoModule.h"
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
// #include <EHnet++/InetAddress.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

// $Revision$
// For change log see ../ChangeLog

int main(int argc, char *argv[])
{
   // InetAddress echoaddr(argv[1], 7);
   // StreamFDModule StdIn(0, StreamFDModule::CheckRead, false);
   // StreamFDModule StdOut(1, StreamFDModule::CheckWrite, false);
   int fd = open("/dev/tty", O_RDWR);
   if (fd >= 0)
   {
      int temp;

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
   }
   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   StreamFDModule fdmod(fd, pm);
   EchoModule echo;

   fdmod.makePlug(0)->plugInto(*(echo.makePlug(0)));

   while (!fdmod.readEOF())
   {
      disp.DispatchEvents(1);
   }
}
