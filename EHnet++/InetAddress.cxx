/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/23 17:45:29  hopper
 // Initial revision
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

 // $Revision$

#ifdef __GNUG__
#  pragma implementation "InetAddress.h"
#endif

#ifndef OS2
#  include <EHnet++/InetAddress.h>
#  include <netdb.h>
#  include <sys/socket.h>
#  include <sys/param.h>
#else
#  include "inetaddr.h"
extern "C" {
#  include <netdb.h>
}
#  include <stdlib.h>
extern "C" {
#  include <sys/socket.h>
#  include <utils.h>
}
#  define MAXHOSTNAMELEN (_HOSTBUFSIZE + 1)
#endif

#include <ctype.h>
#include <string.h>

NetString InetAddress::ToDec(U2Byte num) const
{
   U2Byte top = 10000;
   NetString temp;

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

NetString InetAddress::AsString()
{
   NetString portnum = ToDec(port);
   NetString temp = hostname;

   temp += " port ";
   temp += portnum;

   return(temp);
}

const InetAddress &InetAddress::operator =(const InetAddress &b)
{
   if (this == &b)
      return(*this);
   hostname = b.hostname;
   port = b.port;
   inaddr = b.inaddr;

   return(*this);
}

const InetAddress &InetAddress::operator =(const sockaddr_in &iadr)
{
   struct hostent *hostinfo;

   if (&inaddr != &iadr) {
      inaddr = iadr;
      inaddr.sin_family = AF_INET;
      memset(inaddr.sin_zero, '\0', sizeof(inaddr.sin_zero));
   }
   port = ntohs(iadr.sin_port);
   hostinfo = gethostbyaddr((char *)(&(inaddr.sin_addr)),
			    sizeof(inaddr.sin_addr), AF_INET);
   if (hostinfo == 0) {
      unsigned long addr = ntohl(inaddr.sin_addr.s_addr);

      hostname = ToDec((addr >> 24) & 0xffUL);
      hostname += '.';
      hostname += ToDec((addr >> 16) & 0xffUL);
      hostname += '.';
      hostname += ToDec((addr >> 8) & 0xffUL);
      hostname += '.';
      hostname += ToDec(addr & 0xffUL);
   } else
      hostname = hostinfo->h_name;
   return(*this);
}

extern "C" int atoi(const char *);

void InetAddress::InvalidateAddress()
{
   hostname = NetString("invalid.host.name");

   inaddr.sin_addr.s_addr = INADDR_ANY;
}

int InetAddress::InvalidNum(unsigned long &num, int &i, int enddot)
{
   if (i >= hostname.strlen() || !isdigit(hostname.CharAt(i))) {
      InvalidateAddress();
      return(0);
   }
   
   NetString temphname = hostname;

   num = temphname.atoiAt(i);
   if (num < 0 || num > 255) {
      InvalidateAddress();
      return(1);
   }
   while (i < hostname.strlen() && isdigit(hostname.CharAt(i)))
      i++;
   if (enddot) {
      if (i >= hostname.strlen() || hostname.CharAt(i) != '.') {
	 InvalidateAddress();
	 return(1);
      } else
	 i++;
   } else {
      if (i < hostname.strlen()) {
	 InvalidateAddress();
	 return(1);
      }
   }
   return(0);
}

InetAddress::InetAddress(const NetString &h_name, U2Byte prt) :
        hostname(h_name), port(prt)
{
   struct hostent *hostinfo;
   NetString thname = hostname;

   memset(&inaddr, '\0', sizeof(inaddr));
   inaddr.sin_family = AF_INET;
   inaddr.sin_port = htons(port);
   if (isdigit(hostname.CharAt(0))) {
      unsigned long num1, num2, num3, num4;
      int i;

      i = 0;
      if (InvalidNum(num1, i))
	 return;
      if (InvalidNum(num2, i))
	 return;
      if (InvalidNum(num3, i))
	 return;
      if (InvalidNum(num4, i, 0))
	 return;
      inaddr.sin_addr.s_addr = htonl((((((num1 << 8) | num2) << 8) | num3)
				      << 8) | num4);
      *this = inaddr;
   } else {
      hostinfo = gethostbyname(thname);
      if (hostinfo == 0 || hostinfo->h_addrtype != AF_INET)
	 InvalidateAddress();
      else {
	 inaddr.sin_addr.s_addr =
	    *((unsigned long *)(hostinfo->h_addr_list[0]));
      }
   }
}

static char buf[MAXHOSTNAMELEN + 1];

extern "C" int gethostname(char *, int);

InetAddress::InetAddress(U2Byte prt) : port(prt)
{
   struct hostent *hostinfo;

   memset(&inaddr, '\0', sizeof(inaddr));
   inaddr.sin_family = AF_INET;
   inaddr.sin_port = htons(port);
   inaddr.sin_addr.s_addr = INADDR_ANY;
   gethostname(buf, MAXHOSTNAMELEN + 1);
   hostinfo = gethostbyname(buf);
   if (hostinfo == 0) {
      hostname = "INADDR_ANY(";
      hostname += buf;
      hostname += ")";
   } else {
      hostname = "INADDR_ANY(";
      hostname += hostinfo->h_name;
      hostname += ')';
   }
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
