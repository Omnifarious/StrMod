#ifndef _STR_SocketModule_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/22 04:46:50  hopper
 // Initial revision
 //
 // -> Revision 0.16  1995/04/17  22:54:25  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.15  1995/04/14  17:28:01  hopper
 // -> Combined versions 0.14 and 0.14.0.4
 // ->
 // -> Revision 0.14.0.4  1994/06/16  02:19:40  hopper
 // -> Added #pragma interface declaration.
 // ->
 // -> Revision 0.14.0.3  1994/05/24  04:00:56  hopper
 // -> Added capability to block, or not block on the connect request.
 // ->
 // -> Revision 0.14.0.2  1994/05/22  03:13:18  hopper
 // -> Added the word inline where it should've been all along.
 // ->
 // -> Revision 0.14.0.1  1994/05/16  02:36:55  hopper
 // -> No changes, but I needed a head for the inevitable WinterFire-OS/2
 // -> branch.
 // ->
 // -> Revision 0.14  1994/05/16  02:32:11  hopper
 // -> Superficial changes to make class declaration actually readable. Moved a lot
 // -> of functions that were made inline inside the class declaration into a
 // -> seperate inline are at the bottom.
 // ->
 // -> Revision 0.13  1994/05/07  03:40:55  hopper
 // -> Move enclosing #ifndef ... #define bits around. Changed names of some
 // -> non-multiple include constants. Changed directories for certain
 // -> type of include files.
 // ->
 // -> Revision 0.12  1992/05/17  23:38:05  hopper
 // -> Added some stuff so that class SockListenModule could work.
 // ->
 // -> Revision 0.11  1992/05/02  01:59:13  hopper
 // -> Changed a bit here & there. Added
 // -> SocketModule::GetPeerAddr(), changed definition of
 // -> SocketModule::MakeSocket(SocketAddress &addr) to
 // -> SocketModule::MakeSocket(SocketModule *obj, const SocketAddress &addr),
 // -> and changed constructor to match.
 // ->
 // -> Revision 0.10  1992/05/02  00:47:04  hopper
 // -> Genesis!
 // ->

#ifndef NO_RcsID
static char _SocketModule_H_rcsID[] =
      "$Id$";
#endif

#ifndef _STR_StreamFDModule_H_
#  include <StrMod/StreamFDModule.h>
#endif

#include <EHnet++/SocketAddress.h>

#define _STR_SocketModule_H_

class SocketAddress;

class SocketModule : public StreamFDModule {
   friend class SockListenModule;  // This is so SockListenModule's can access
                                   // that one protected constructor down there
 public:
   static const STR_ClassIdent identifier;

   inline SocketModule(const SocketAddress &addr, bool hangdelete = true,
		       bool blockconnect = true);
   inline virtual ~SocketModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   SocketAddress *GetPeerAddr()                        { return(peer); }

 protected:
   SocketAddress *peer;

   inline SocketModule(int fd, SocketAddress *pr);

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   static int MakeSocket(SocketModule *obj,
			 const SocketAddress &addr, int blockconnect);
};

//-------------------------------inline functions------------------------------

inline SocketModule::SocketModule(const SocketAddress &addr,
				  bool hangdelete, bool blockconnect)
 :   StreamFDModule(MakeSocket(this, addr, blockconnect),
		    StreamFDModule::CheckBoth,  hangdelete)
{
   peer = addr.Copy();
}

inline SocketModule::~SocketModule()  // This might be changed later to add
{                                     // a shutdown message sent to the
   delete peer;                       // socket on the other side of the
}                                     // connection.

inline SocketModule::SocketModule(int fd, SocketAddress *pr)
 :   StreamFDModule(fd), peer(pr)
{
}

inline int SocketModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamFDModule::AreYouA(cid));
}

#endif
