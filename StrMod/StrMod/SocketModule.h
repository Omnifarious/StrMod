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
class SockListenModule;

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
   //: An EOF indication has been written.  This function has to handle it.
   // This function follows the template method pattern from Design Patterns.
   // Return true if the fd can now be written to, and return false if it
   // can't.  It would probably also be best to set some kind of error
   // condition if it can't be written to.
   virtual bool writeEOF();

   //: Create a SocketModule using the given fd
   // Note that ownership of pr is being passed.
   SocketModule(int fd, SocketAddress *pr, UNIXpollManager &pollmgr);

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   static int MakeSocket(SocketModule &obj,
			 const SocketAddress &addr, bool blockconnect);

 private:
   SocketAddress &peer;
   int makesock_errno;
};

//-------------------------------inline functions------------------------------

inline int SocketModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamFDModule::AreYouA(cid));
}

#endif
