/* $Header$ */

// See ChangeLog file for log

#include "UniEvent/SimpleDispatcher.h"
#include "testdisp.h"
#include <iostream.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <seed value>\n";
      return(1);
   } else {
      UNISimpleDispatcher disp;

      TestDispatcher(disp, atoi(argv[1]));
   }
   return(0);
}
