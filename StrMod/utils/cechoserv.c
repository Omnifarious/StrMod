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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

static char buf[4096];

main(int argc, char *argv[])
{
   int sd, length, fd;
   struct sockaddr_in inaddr;

   if (argc != 2) {
      fprintf(stderr, "Usage: %s <socket #>\n", argv[0]);
      exit(2);
   }
   if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      exit(1);
   }
   inaddr.sin_family = AF_INET;
   inaddr.sin_port = htons(atoi(argv[1]));
   inaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
   length = sizeof(inaddr);
   if (bind(sd, (struct sockaddr *)(&inaddr), length) < 0) {
      perror("bind");
      exit(1);
   }
   if (listen(sd, 1) < 0) {
      perror("listen");
      exit(1);
   }
   while (1) {
      long addr;

      length = sizeof(inaddr);
      if ((fd = accept(sd, (struct sockaddr *)(&inaddr), &length)) < 0) {
	 perror("accept");
	 exit(1);
      }
      addr = ntohl(inaddr.sin_addr.S_un.S_addr);
      printf("Got connection from %d.%d.%d.%d\n",
	     (addr >> 24) & 0xff,
	     (addr >> 16) & 0xff,
	     (addr >> 8)  & 0xff,
	     addr         & 0xff);
      while ((length = read(fd, buf, 4096)) > 0) {
	 int i;

	 write(fileno(stdout), buf, length);
	 printf("Wheee! %d\n", write(fd, buf, length));
      }
      close(fd);
      printf("--** Connection closed **--\n");
   }
}
