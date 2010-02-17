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

#include <fcntl.h>
#include <stdio.h>

static char buf[4096];

int main(int argc, char *argv[])
{
   int length, i, o, c, fd, audio, temp, divider;

   if (argc != 3) {
      fprintf(stderr, "Usage: test <filename> <divider>\n");
      exit(1);
   }
   if ((fd = open(argv[1], O_RDONLY)) < 0) {
      perror("test");
      exit(1);
   }
   divider = atoi(argv[2]);
   if ((audio = open("/dev/audio", O_WRONLY)) < 0) {
      fprintf(stderr, "Error opening /dev/audio\n");
      perror("test");
   }
   while ((length = read(fd, buf, 4096)) > 0) {
      for (i = 0, o = 0, c = 0; i < length; i++) {
	 buf[o] = buf[i];
	 if (++c != divider)
	    o++;
	 else
	    c = 0;
      }
      printf("length == %d && o == %d\n", length, o);
      length = o;
      while (length > 0) {
	 if ((temp = write(audio, buf, length)) < 0) {
	    fprintf(stderr, "Error writing to /dev/audio\n");
	    perror("test");
	 }
	 length -= temp;
      }
   }
   close(fd);
   printf("I'm here!\n");
   close(audio);
   return(0);
}
