#include <string>
#include <EHnet++/InetAddress.h>
#include <iostream.h>

int main(int argc, char *argv[])
{
   if (argc <= 1) {
      InetAddress iaddr(0);

      cout << "iaddr == \"" << iaddr.AsString() << "\"\n";
   } else {
      string name = argv[1];
      InetAddress iaddr(name, 0);

      cout << "iaddr == \"" << iaddr.AsString() << "\"\n";
   }
   return(0);
}
