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
