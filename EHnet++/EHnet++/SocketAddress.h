#ifndef _EHNET_SocketAddress_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/23 17:45:30  hopper
 // Initial revision
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

#define _EHNET_SocketAddress_H_

struct sockaddr;
class ostream;
class NetString;

class SocketAddress {
 protected:
   virtual SocketAddress *MakeCopy() const = 0;

 public:
   virtual void PrintOn(ostream &);

   virtual struct sockaddr *SockAddr() = 0;
   SocketAddress *Copy() const                  { return(MakeCopy()); }
   virtual int AddressSize() const = 0;
   virtual NetString AsString() = 0;

   SocketAddress()                              { }
   virtual ~SocketAddress()                     { }
};

#endif
