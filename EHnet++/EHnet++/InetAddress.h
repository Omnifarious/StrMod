#ifndef _EHNET_InetAddress_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

 // For log, see ../ChangeLog

 // $Revision$

 // Revision 1.3  1996/02/12 03:01:53  hopper
 // Added links to my ClassIdent system.
 //
 // Revision 1.2  1996/02/12 00:32:55  hopper
 // Fixed to use the new C++ standard library string class instead of all the
 // 'NetString' silliness.
 //
 // Revision 1.1.1.1  1995/07/23 17:45:30  hopper
 // Imported sources
 //
 // Revision 0.2.0.4.1.1  1995/07/23 03:21:38  hopper
 // Added #include <string.h> so it would compile correctly used libg++ 2.7.0.
 // libg++ 2.7.0 is in error.
 //
 // Revision 0.2.0.4  1994/08/12 17:07:12  hopper
 // Changed to use NetString class. The NetString class handles all library
 // dependencies.
 //
 // Revision 0.2.0.3  1994/06/16  02:59:39  hopper
 // Added #pragma interface for GNU stuff.
 //
 // Revision 0.2.0.2  1994/05/08  18:34:52  hopper
 // Changed to work better with Rogue Wave classes.
 //
 // Revision 0.2.0.1  1994/05/08  18:06:21  hopper
 // Head of WinterFire branch. Changed all instances of String with
 // RWCString.
 //
 // Revision 0.2  1994/05/08  16:11:06  hopper
 // Shifted declarations and definitions to work better with new libraries.
 //
 // Revision 0.1  1994/05/03  03:25:21  hopper
 // Initial revision
 //

#ifndef _EHNET_SocketAddress_H_
#   include <EHnet++/SocketAddress.h>
#endif

#include <string>
#include <sys/types.h>
#include <netinet/in.h>

#define _EHNET_InetAddress_H_

typedef unsigned short U2Byte;

class InetAddress : public SocketAddress {
 public:
   static const NET_ClassIdent identifier;

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || SocketAddress::AreYouA(cid));
   }

   virtual struct sockaddr *SockAddr(){ return((struct sockaddr *)(&inaddr)); }
   InetAddress *Copy() const          { return((InetAddress *)MakeCopy()); }
   virtual int AddressSize() const    { return(sizeof(sockaddr_in)); }
   virtual string AsString();

   string GetHostname() const         { return(hostname); }
   string GetHostname(bool forcelookup);
   U2Byte GetPort() const             { return(port); }

   const InetAddress &operator =(const InetAddress &b);
   const InetAddress &operator =(const sockaddr_in &iadr);

   InetAddress(const string &h_name, U2Byte prt);
   InetAddress(U2Byte port);
   InetAddress(const InetAddress &b);
   InetAddress(const sockaddr_in &iadr);
   virtual ~InetAddress()             { }

 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

   inline virtual SocketAddress *MakeCopy() const;

   void InvalidateAddress();
   static bool ParseNumeric(const char *numeric_addr, unsigned int &num);
   static bool NameToIaddr(const char *name_addr, unsigned int &num);
   static string IaddrToName(const sockaddr_in &inaddr);

 private:
   string hostname;
   U2Byte port;
   sockaddr_in inaddr;

   static bool AsciiToQInum(const char *s, int &i,
			    unsigned long &num, bool endsindot);
   static string ToDec(U2Byte num);
};

//--------------------------------inline functions-----------------------------

inline const ClassIdent *InetAddress::i_GetIdent() const
{
   return(&identifier);
}

inline SocketAddress *InetAddress::MakeCopy() const
{
   return(new InetAddress(*this));
}

#endif
