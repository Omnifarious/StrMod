/* $Header$ */

// For log, see ./ChangeLog
//
// $Revision$
//
 // Revision 1.2  1996/02/12 00:32:36  hopper
 // Fixed to use the new C++ standard library string class instead of all the
 // 'NetString' silliness.
 //
 // Revision 1.1.1.1  1995/07/23 17:45:29  hopper
 // Imported sources
 //
 // Revision 0.3  1995/01/06  15:56:26  hopper
 // Merged 0.2 and 0.2.0.6
 //
 // Revision 0.2.0.6  1995/01/06  15:54:48  hopper
 // Added stuff to make this work (hopefully) under OS/2.
 //
 // Revision 0.2.0.5  1994/08/12  17:05:40  hopper
 // Changed to use NetString class. The NetString class handles all library
 // dependencies.
 //
 // Revision 0.2.0.4  1994/07/18  03:30:08  hopper
 // Added a #pragma implementation line so it would work better with gcc 2.6.0
 //
 // Revision 0.2.0.3  1994/05/26  04:22:19  hopper
 // Made even more changes to make this work with RWCString class.
 // Used htons, ntohs, htonl, and ntohl in a more consistent fashion. I'm on a
 // big endian machine now!
 //
 // Revision 0.2.0.2  1994/05/08  18:33:06  hopper
 // Changed to work better with Rogue Wave classes.
 //
 // Revision 0.2.0.1  1994/05/08  17:58:23  hopper
 // Head of WinterFire branch. Changed all instances of String with
 // RWCString.
 //
 // Revision 0.2  1994/05/08  16:10:57  hopper
 // Shifted declarations and definitions to work better with new libraries.
 //
 // Revision 0.1  1994/05/03  03:23:38  hopper
 // Initial revision.
 //

#ifdef __GNUG__
#  pragma implementation "InetAddress.h"
#endif

#include <EHnet++/InetAddress.h>
#include <ctype.h>
#include <sys/socket.h>
#include <stdlib.h>

string InetAddress::AsString()
{
   string portnum = ToDec(port);
   string temp = hostname;

   /* I've seen a few alternative repesentations.  I guess the most common
      is the same as this, except with no "port ". */

   temp += " port ";
   temp += portnum;

   return(temp);
}

string InetAddress::GetHostname(bool forcelookup)
{
   if (forcelookup) {
      hostname = IaddrToName(inaddr);
      return(hostname);
   } else {
      return(GetHostname());
   }
}

const InetAddress &InetAddress::operator =(const InetAddress &b)
{
   /* If we're not ourselves, copy all the data over from the other
      InetAddress. */
   if (this != &b) {
      hostname = b.hostname;
      port = b.port;
      inaddr = b.inaddr;
   }

   return(*this);
}

const InetAddress &InetAddress::operator =(const sockaddr_in &iadr)
{
   struct hostent *hostinfo;

   /* Make sure we're not being set to our own inaddr, so the = won't be
      disastrous. */
   if (&inaddr != &iadr) {
      inaddr = iadr;
   }

   /* Make sure we look a lot like the OS thinks an inaddr should look. */
   inaddr.sin_family = AF_INET;
   memset(inaddr.sin_zero, '\0', sizeof(inaddr.sin_zero));

   /* Cache the hostname and port info.  We could instead do this every time
      someone asks, but it would be kind of wasteful. */
   port = ntohs(iadr.sin_port);
   hostname = IaddrToName(inaddr);

   return(*this);
}

InetAddress::InetAddress(const string &h_name, U2Byte prt) :
        hostname(h_name), port(prt)
{
   const char *c_hostname;

   c_hostname = h_name.c_str();
   memset(&inaddr, '\0', sizeof(inaddr));
   inaddr.sin_family = AF_INET;
   inaddr.sin_port = htons(port);

   if (ParseNumeric(c_hostname, inaddr.sin_addr.s_addr)) {
      /* If the address made sense as a <num>.<num>.<num>.<num> address,
	 attempt to look up a 'real' hostname for this address. */
      hostname = IaddrToName(inaddr);

   } else if (NameToIaddr(c_hostname, inaddr.sin_addr.s_addr)) {
      /* Otherwise, if we were able to translate the name into an actual
         internet address, then the hostname is the name passed in. See note
         after function. */
      hostname = h_name;

   } else {
      /* This doesn't seem like any address we can translate to an internet
         address. */
      InvalidateAddress();
   }
}
/* **Note about above function**
   This is, perhaps, not the right way to do things because the passed in
   name could simply be an alias.  I think this way causes fewer surprises
   for the programmer using this class, and there ARE ways to force lookup
   of the alias, namely by calling GetHostname(true) to force a lookup. */

InetAddress::InetAddress(U2Byte prt) : port(prt)
{
   struct hostent *hostinfo;

   memset(&inaddr, '\0', sizeof(inaddr));
   inaddr.sin_family = AF_INET;
   inaddr.sin_port = htons(port);
   inaddr.sin_addr.s_addr = INADDR_ANY;
   hostname = IaddrToName(inaddr);
}

InetAddress::InetAddress(const InetAddress &b) : hostname(b.hostname),
                                                 port(b.port),
                                                 inaddr(b.inaddr)
{
}

InetAddress::InetAddress(const sockaddr_in &iadr) : port(0)
{
   memset(&inaddr, '\0', sizeof(inaddr));
   *this = iadr;
}

void InetAddress::InvalidateAddress()
{
   hostname = string("invalid.host.name");

   inaddr.sin_addr.s_addr = INADDR_ANY;
}

bool InetAddress::ParseNumeric(const char *numeric_addr, unsigned int &num)
{
   if (isdigit(numeric_addr[0])) {
      unsigned long num1, num2, num3, num4;
      int i;

      i = 0;
      if (!AsciiToQInum(numeric_addr, i, num1, true))
	 return(false);
      if (!AsciiToQInum(numeric_addr, i, num2, true))
	 return(false);
      if (!AsciiToQInum(numeric_addr, i, num3, true))
	 return(false);
      if (!AsciiToQInum(numeric_addr, i, num4, false))
	 return(false);
      num = htonl((((((num1 << 8) | num2) << 8) | num3) << 8) | num4);
      return(true);
   } else {
      return(false);
   }
}

bool InetAddress::AsciiToQInum(const char *s, int &i,
			       unsigned long &num, bool endsindot)
{
   int mynum;

   if (!isdigit(s[i])) {
      return(false);
   }
   
   mynum = atoi(s + i);
   if (mynum < 0 || mynum > 255) {
      return(false);
   }
   while (s[i] != '\0' && isdigit(s[i]))
      i++;
   if (endsindot) {
      if (s[i] != '.') {
	 return(false);
      } else {
	 i++;
      }
   } else {
      if (s[i] != '\0') {
	 return(false);
      }
   }
   num = mynum;
   return(true);
}

string InetAddress::ToDec(U2Byte num)
{
   U2Byte top = 10000;
   string temp;

   while (top > 0 && (num / top) < 1)
      top /= 10;
   if (top <= 0)
      temp += '0';
   else {
      while (top > 0) {
	 temp += '0' + ((num / top) % 10);
	 top /= 10;
      }
   }
   return(temp);
}
