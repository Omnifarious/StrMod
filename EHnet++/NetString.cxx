/* $Header$ */

// $Log$
// Revision 1.1  1995/07/23 17:45:29  hopper
// Initial revision
//
// Revision 0.3  1995/01/06  16:32:57  hopper
// Added some stuff so this will work (hopefully) under OS/2.
//
// Revision 0.2  1995/01/06  16:29:03  hopper
// Added gethostbyname utility function. Fixed a few syntax errors and typos.
//
// Revision 0.1  1994/08/12  06:32:16  hopper
// Genesis!
//

// $Revision$

#ifdef __GNUG__
#  pragma implementation "NString.h"
#endif

#ifndef OS2
#  include <EHnet++/NString.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#else
#  include "nstring.h"
   extern "C" {
#  include <sys/types.h>
#  include <sys/socket.h>
#  ubckyde <utils.h>
#  include <netdb.h>
   }
#endif

#include <assert.h>
#include <stdlib.h>
#include <iostream.h>

#ifdef USE_RWSTRING

int NetString::atoiAt(int index)
{
   assert(index >= 0 && index < strlen());

   return(atoi((const char *)(implstr) + index));
}

struct hostent *gethostbyname(const NetString &str)
{
   RWCString temp = str.implstr;

   return(gethostbyname((const char *)(temp)));
}

ostream &operator <<(ostream &os, const NetString &str)
{
   os << str.implstr;
   return(os);
}

#elif defined(USE_EHSTRING)

int NetString::atoiAt(S2Byte index)
{
   assert(index >= 0 && index < strlen());

   return(atoi((const char *)(*this) + index));
}

struct hostent *gethostbyname(const NetString &str)
{
   String temp = str;

   return(gethostbyname(temp));
}

ostream &operator <<(ostream &os, const NetString &str)
{
   str.PrintOn(os);
   return(os);
}

#else

#include <string.h>

void NetString::SetString(const char *s, int slen)
{
   if (string) {
      delete [] string;
      len = 0;
   }
   if (slen > 0) {
      string = new char[slen + 1];
      strcpy(string, s);
      len = slen;
   }
}

void NetString::SetNewString(char *s, int slen)
{
   if (string) {
      delete [] string;
      len = 0;
   }
   if (slen > 0) {
      string = s;
      len = slen;
   }
}

void NetString::AddString(const char *s, int slen)
{
   if (slen <= 0)
      return;
   else if (string == 0)
      SetString(s, slen);
   else {
      char *newstr = new char[slen + len + 1];

      strcpy(newstr, string);
      strcpy(newstr + len, s);
      SetNewString(newstr, slen + len);
   }
}

NetString::NetString(const char *s) : string(0), len(0)
{
   int slen;

   if (s && (slen = ::strlen(s)) > 0)
      SetString(s, slen);
}

NetString::NetString(const NetString &str) : string(0), len(0)
{
   SetString(str.string, str.strlen());
}

NetString::~NetString()
{
   if (string)
      delete [] string;
}

const char NetString::CharAt(int index) const
{
   assert(index >= 0 && index < strlen());

   return(string[index]);
}

void NetString::SetCharTo(int index, char c)
{
   assert(index >= 0 && index < strlen());

   string[index] = c;
}

const NetString &NetString::operator +=(char c)
{
   char buf[2];

   assert(c != '\0');

   buf[0] = c;
   buf[1] = '\0';
   AddString(buf, 1);
}

const NetString &NetString::operator +=(const char *s)
{
   if (s && *s)
      AddString(s, ::strlen(s));
   return(*this);
}

const NetString &NetString::operator +=(const NetString &str)
{
   if (str.strlen() > 0)
      AddString(str.string, str.strlen());
   return(*this);
}

const NetString &NetString::operator =(char c)
{
   char buf[2];

   assert(c != '\0');

   buf[0] = c;
   buf[1] = '\0';
   SetString(buf, 1);
}

const NetString &NetString::operator =(const char *s)
{
   if (s && *s)
      SetString(s, ::strlen(s));
   else
      SetString("", 0);
}

const NetString &NetString::operator =(const NetString &str)
{
   if (str.strlen() > 0)
      SetString(str.string, str.strlen());
   else
      SetString("", 0);
}

int NetString::atoiAt(int index)
{
   assert(index >= 0 && index < strlen());

   if (string)
      return(atoi(string + index));
   else
      return(0);
}

struct hostent *gethostbyname(const NetString &str)
{
   if (str.strlen() > 0)
      return(gethostbyname(str.string));
   else
      return(gethostbyname(""));
}

ostream &operator <<(ostream &os, const NetString &str)
{
   os << str.string;
   return(os);
}

#endif
