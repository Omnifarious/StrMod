#ifndef _EHNET_SocketAddress_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

 // $Log$
 // Revision 1.4  1996/02/20 01:06:33  hopper
 // Added inline definition for ostream &operator << for SocketAddresses.
 //
 // Revision 1.3  1996/02/12 03:01:55  hopper
 // Added links to my ClassIdent system.
 //
 // Revision 1.2  1996/02/12 00:32:57  hopper
 // Fixed to use the new C++ standard library string class instead of all the
 // 'NetString' silliness.
 //
 // Revision 1.1.1.1  1995/07/23 17:45:30  hopper
 // Imported sources
 //
 // Revision 0.1.0.4  1994/08/12  17:07:26  hopper
 // Changed to use NetString class. The NetString class handles all library
 // dependencies.
 //
 // Revision 0.1.0.3  1994/06/16  03:00:06  hopper
 // Added #pragma interface for GNU stuff.
 //
 // Revision 0.1.0.2  1994/05/08  18:35:08  hopper
 // Changed to work better with Rogue Wave classes.
 //
 // Revision 0.1.0.1  1994/05/08  18:10:25  hopper
 // Head of WinterFire branch. Changed all instances of String with
 // RWCString.
 //
 // Revision 0.1  1994/05/03  03:25:21  hopper
 // Initial revision
 //

 // $Revision$

#include <string>
#include <LCore/Protocol.h>
#include <EHnet++/NET_ClassIdent.h>

#define _EHNET_SocketAddress_H_

struct sockaddr;
class ostream;

class SocketAddress : virtual public Protocol {
 public:
   static const NET_ClassIdent identifier;

   SocketAddress()                              { }
   virtual ~SocketAddress()                     { }

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || Protocol::AreYouA(cid));
   }

   virtual void PrintOn(ostream &);

   virtual struct sockaddr *SockAddr() = 0;
   SocketAddress *Copy() const                  { return(MakeCopy()); }
   virtual int AddressSize() const = 0;
   virtual string AsString() = 0;

 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

   virtual SocketAddress *MakeCopy() const = 0;
};

//-----------------------------inline functions--------------------------------

inline const ClassIdent *SocketAddress::i_GetIdent() const
{
   return(&identifier);
}

inline ostream &operator <<(ostream &os, SocketAddress &sa)
{
   sa.PrintOn(os);
   return(os);
}

#endif
