#ifndef _OS2SocketModule_H_

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
// Revision 1.2  1995/01/13  17:22:57  hopper
// Added stuff to make OS2SockListenModule work.
//
// Revision 1.1  1995/01/05  21:43:32  hopper
// Initial revision
//

// $Revision$

#ifndef NO_RcsID
static char _OS2SocketModule_H_rcsID[] =
      "$Id$";
#endif

#ifndef _EH_StreamModule_H_
#   include "strmod.h"
#endif

#ifndef dp_iohandler_h
#   include "iohandle.h"
#endif

#define _OS2SocketModule_H_

class OS2SockPlug;
class SocketAddress;
class OS2SockListenModule;

//---------------------------class OS2SocketModule-----------------------------

class OS2SocketModule : public StreamModule {
   friend class OS2SockPlug;
   friend class OS2SockListenModule;

 protected:
   int sd, last_error;
   struct {
      unsigned int plugmade   : 1;
      unsigned int hangdelete : 1;
      unsigned int checkread  : 1;
      unsigned int checkwrite : 1;
   } flags;
   OS2SockPlug *plug;
   StrChunk *cur_write, *buffed_read;
   int write_pos;
   unsigned int max_block_size;
   SocketAddress *peer;

   static int MakeSocket(OS2SocketModule *obj,
			 const SocketAddress &addr, int blockconnect);

   inline virtual StrPlug *CreatePlug(int side);

   OS2SocketModule(int sockfd, const SocketAddress &addr,
                   unsigned int hangdelete = 1);

 public:
   inline virtual int CanCreate(int side) const;
   inline OS2SockPlug *MakePlug(int side);
   inline virtual int OwnsPlug(StrPlug *p) const;
   inline virtual int DeletePlug(StrPlug *p);

   virtual int ModuleType() const       { return(OS2SocketModuleNum); }
   virtual int AreYouA(int mtype) const {
      return((mtype == OS2SocketModuleNum) || StreamModule::AreYouA(mtype));
   }

   int HasError() const                      { return(last_error != 0); }
   int ErrorNum() const                      { return(last_error); }
   void ClearError()                         { last_error = 0; }
   char *ErrorString() const;
   int BestChunkSize();
   void SetMaxBlockSize(unsigned int mbs)    { max_block_size = mbs; }
   void UnsetMaxBlockSize()                  { max_block_size = 0; }
   unsigned int GetMaxBlockSize() const      { return(max_block_size); }

   SocketAddress *GetPeerAddr()              { return(peer); }

   OS2SocketModule(const SocketAddress &addr, unsigned int hangdelete = 1,
		   unsigned int blockconnect = 1);
   virtual ~OS2SocketModule();
};

//-----------------------------class OS2SockPlug-------------------------------

class OS2SockPlug : public StrPlug, public IOHandler {
 protected:
   OS2SocketModule *smod;

   virtual StreamModule *ParentModule() const { return(smod); }
   virtual StrChunk *InternalRead();

   virtual void ReadableNotify();
   virtual void WriteableNotify();

   virtual int inputReady(int fd);
   virtual int outputReady(int fd);
   virtual int exceptionRaised(int fd);
   virtual void timerExpired(long sec, long usec);

 public:
   virtual int PlugType() const         { return(OS2SockPlugNum); }
   virtual int AreYouA(int ptype) const {
      return((ptype == OS2SockPlugNum) || StrPlug::AreYouA(ptype));
   }

   virtual int CanWrite() const;
   virtual int Write(StrChunk *);

   inline virtual int CanRead() const;

   inline OS2SocketModule *ModuleFrom() const;
   inline virtual int Side() const;

   OS2SockPlug(OS2SocketModule *parent);
   virtual ~OS2SockPlug();
};

//----------------inline functions for class OS2SocketModule-------------------
inline StrPlug *OS2SocketModule::CreatePlug(int side)
{
   if (CanCreate(side)) {
      flags.plugmade = 1;
      return(plug);
   } else
      return(0);
}

inline int OS2SocketModule::CanCreate(int side) const
{
   return((side != 0) && !flags.plugmade);
}

inline OS2SockPlug *OS2SocketModule::MakePlug(int side)
{
      return((OS2SockPlug *)(CreatePlug(side)));
}

inline int OS2SocketModule::OwnsPlug(StrPlug *p) const
{
   return(flags.plugmade && p == plug);
}

inline int OS2SocketModule::DeletePlug(StrPlug *p)
{
   if (OwnsPlug(p)) {
      flags.plugmade = 0;
      return(1);
   } else
      return(0);
}


//------------------inline functions for class OS2SockPlug---------------------

inline OS2SocketModule *OS2SockPlug::ModuleFrom() const
{
   return((OS2SocketModule *)ParentModule());
}

inline int OS2SockPlug::Side() const
{
   return(1);
}

inline int OS2SockPlug::CanWrite() const
{
   return(ModuleFrom()->cur_write == 0);
}

inline int OS2SockPlug::CanRead() const
{
   return(ModuleFrom()->buffed_read != 0);
}

#endif
