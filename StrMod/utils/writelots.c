#include <unistd.h>

static char block[8192];

main()
{
   int result;

   memset(block, 'a', 8192);
   do {
      result = write(1, block, 8192);
   } while (result > 0);
}
