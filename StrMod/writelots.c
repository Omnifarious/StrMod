#include <unistd.h>

static char block[8192];

main()
{
   int i;

   for (i = 0; i < 8192; i++)
      block[i] = 'a';
   while (1)
      write(1, block, 8192);
}
