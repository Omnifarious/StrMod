#ifndef _EHNET_String_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/23 17:45:30  hopper
// Initial revision
//
// Revision 0.3  1994/08/12  16:34:05  hopper
// Fixed some minor syntax errors.
//
// Revision 0.2  1994/08/12  06:33:14  hopper
// Added some private utility functions to the NetString class that's used if
// you aren't using any libraries.
//
// Revision 0.1  1994/08/12  05:13:29  hopper
// Genesis!
//

// $Revision$

struct hostent;
class ostream;

#ifdef USE_RWSTRING

#include <rw/cstring.h>

class NetString {

friend struct hostent *gethostbyname(const NetString &str);
friend ostream &operator <<(ostream &os, const NetString &str);

   RWCString implstr;

 public:
   inline NetString();
   inline NetString(const char *str);
   inline NetString(const NetString &str);
   inline NetString(const RWCString &str);
   inline virtual ~NetString();

   inline const char CharAt(int index) const;
   inline void SetCharTo(int index, char c);

   inline const NetString &operator +=(char c);
   inline const NetString &operator +=(const char *s);
   inline const NetString &operator +=(const RWCString &str);
   inline const NetString &operator +=(const NetString &str);

   inline const NetString &operator =(char c);
   inline const NetString &operator =(const char *s);
   inline const NetString &operator =(const RWCString &str);
   inline const NetString &operator =(const NetString &str);

   inline int strlen() const;
   int atoiAt(int index);
};

struct hostent *gethostbyname(const NetString &str);

ostream &operator <<(ostream &os, const NetString &str);

//-------------------------------inline functions------------------------------

inline NetString::NetString()
{
}

inline NetString::NetString(const char *str) : implstr(str)
{
}

inline NetString::NetString(const NetString &str) : implstr(str.implstr)
{
}

inline NetString::NetString(const RWCString &str) : implstr(str)
{
}

inline NetString::~NetString()
{
}

inline const char NetString::CharAt(int index) const
{
   return(implstr(index));
}

inline void NetString::SetCharTo(int index, char c)
{
   implstr(index) = c;
}

inline const NetString &NetString::operator +=(char c)
{
   char buf[2];

   buf[0] = c;
   buf[1] = '\0';
   implstr += buf;
   return(*this);
}

inline const NetString &NetString::operator +=(const char *s)
{
   implstr += s;
   return(*this);
}

inline const NetString &NetString::operator +=(const RWCString &str)
{
   implstr += str;
   return(*this);
}

inline const NetString &NetString::operator +=(const NetString &str)
{
   implstr += str.implstr;
   return(*this);
}

inline const NetString &NetString::operator =(char c)
{
   char buf[2];

   buf[0] = c;
   buf[1] = '\0';
   implstr = buf;
   return(*this);
}

inline const NetString &NetString::operator =(const char *s)
{
   implstr = s;
   return(*this);
}

inline const NetString &NetString::operator =(const RWCString &str)
{
   implstr = str;
   return(*this);
}

inline const NetString &NetString::operator =(const NetString &str)
{
   implstr = str.implstr;
   return(*this);
}

inline int NetString::strlen() const
{
   return(implstr.length());
}

#elif defined(USE_EHSTRING)

//--------------------------class NetString : public String

#include <ibmpp/String.h>

class NetString : public String {
 public:
   inline NetString();
   inline NetString(const char *str);
   inline NetString(const String &str);
   inline NetString(const NetString &str);

   inline void SetCharTo(unsigned int index, char c);

   inline const NetString &operator +=(char c);
   inline const NetString &operator +=(const char *s);
   inline const NetString &operator +=(const String &str);
   inline const NetString &operator +=(const NetString &str);

   inline const NetString &operator =(char c);
   inline const NetString &operator =(const char *s);
   inline const String &operator =(const String &str);
   inline const NetString &operator =(const NetString &str);

   int atoiAt(S2Byte index);
};

struct hostent *gethostbyname(const NetString &str);

ostream &operator <<(ostream &os, const NetString &str);

//-------------------------------inline functions------------------------------

inline NetString::NetString()
{
}

inline NetString::NetString(const char *str) : String(str)
{
}

inline NetString::NetString(const String &str) : String(str)
{
}

inline NetString::NetString(const NetString &str) : String(str)
{
}

inline void NetString::SetCharTo(unsigned int index, char c)
{
   operator [](index) = c;
}

inline const NetString &NetString::operator +=(char c)
{
   String::operator +=(c);
   return(*this);
}

inline const NetString &NetString::operator +=(const char *s)
{
   String::operator +=(s);
   return(*this);
}

inline const NetString &NetString::operator +=(const String &str)
{
   String::operator +=(str);
   return(*this);
}

inline const NetString &NetString::operator +=(const NetString &str)
{
   String::operator +=(str);
   return(*this);
}

inline const NetString &NetString::operator =(char c)
{
   String::operator =(c);
   return(*this);
}

inline const NetString &NetString::operator =(const char *s)
{
   String::operator =(s);
   return(*this);
}

inline const String &NetString::operator =(const String &str)
{
   String::operator =(str);
   return(*this);
}

inline const NetString &NetString::operator =(const NetString &str)
{
   String::operator =(str);
   return(*this);
}

#else

class NetString {

friend struct hostent *gethostbyname(const NetString &str);
friend ostream &operator <<(ostream &os, const NetString &str);

   char *string;
   int len;

   void SetString(const char *s, int slen);
   void SetNewString(char *s, int slen);
   void AddString(const char *s, int slen);

 public:
   inline NetString();
   NetString(const char *s);
   NetString(const NetString &str);
   virtual ~NetString();

   const char CharAt(int index) const;
   void SetCharTo(int index, char c);

   const NetString &operator +=(char c);
   const NetString &operator +=(const char *s);
   const NetString &operator +=(const NetString &str);

   const NetString &operator =(char c);
   const NetString &operator =(const char *s);
   const NetString &operator =(const NetString &str);

   inline int strlen() const;
   int atoiAt(int index);
};

struct hostent *gethostbyname(const NetString &str);

ostream &operator <<(ostream &os, const NetString &str);

//-------------------------------inline functions------------------------------

inline NetString::NetString() : string(0), len(0)
{
}

inline int NetString::strlen() const
{
   return(len);
}

#endif

#endif
