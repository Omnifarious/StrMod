#ifndef _STR_SocketModule_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for a change log.

#ifndef _STR_StreamFDModule_H_
#  include <StrMod/StreamFDModule.h>
#endif

#define _STR_SocketModule_H_

class SocketAddress;

class SocketModule : public StreamFDModule {
   friend class SockListenModule;  // This is so SockListenModule's can access
                                   // that one protected constructor down there
 public:
   static const STR_ClassIdent identifier;

   //: Create a SocketModule connected to the given address.
   SocketModule(const SocketAddress &addr, UNIXpollManager &pollmgr,
		bool hangdelete = true, bool blockconnect = true);
   virtual ~SocketModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: Who are we connected to?
   const SocketAddress &GetPeerAddr()                  { return(peer); }

 protected:
   SocketAddress &peer;
   int makesock_errno;

   //: Create a SocketModule using the given fd
   SocketModule(int fd, SocketAddress *pr, UNIXpollManager &pollmgr);

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   static int MakeSocket(SocketModule &obj,
			 const SocketAddress &addr, bool blockconnect);
};

//-------------------------------inline functions------------------------------

inline int SocketModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamFDModule::AreYouA(cid));
}

#endif
