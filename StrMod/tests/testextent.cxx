/* $Header$ */

#include <StrMod/LinearExtent.h>
#include <iostream.h>

ostream &operator <<(ostream &os, LinearExtent &ext)
{
   os << '[' << ext.Offset() << '_' << ext.Length() << "_)";
}

LinearExtent TestExtent()
{
   LinearExtent joe(0, 30);
   LinearExtent george(joe.SubExtent(LinearExtent(10, 5)));

   cout << joe << ", " << george << '\n';
   george.LengthenRight(5);
   cout << "___george___\n";
   cout << george << '\n';
   george.LengthenLeft(5);
   cout << george << '\n';
   george.LengthenCenter(5);
   cout << george << '\n';
   joe.ShortenRight(5);
   cout << "___joe___\n";
   cout << joe << '\n';
   joe.ShortenLeft(5);
   cout << joe << '\n';
   joe.LengthenCenter(5);
   cout << joe << '\n';
   joe.ShortenCenter(5);
   cout << joe << '\n';
   joe.ShortenCenter(5);
   cout << joe << '\n';
   return(joe.SubExtent(george));
}

main()
{
   LinearExtent test = TestExtent();

   cout << "test == " << test << '\n';
}
