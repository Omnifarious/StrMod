#ifndef _SockListenModule_H_

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
 // -> Revision 0.11.0.2  1994/05/16  01:54:49  hopper
 // -> Added RCS Revision comment.
 // ->
 // -> Revision 0.11.0.1  1994/05/16  01:50:01  hopper
 // -> Changed String to RWCString to work with Rogue Wave classes.
 // ->
 // -> Revision 0.11  1994/05/07  03:40:55  hopper
 // -> Move enclosing #ifndef ... #define bits around. Changed names of some
 // -> non-multiple include constants. Changed directories for certain
 // -> type of include files.
 // ->
 // -> Revision 0.10  1992/05/17  22:40:28  hopper
 // -> Genesis!
 // ->

// $Revision$

#ifndef NO_RcsID
static char _SockListenModule_H_rcsID[] =
      "$Id$";
#endif

#ifndef _EH_SocketModule_H_
#   include <StrMod/SocketModule.h>
#endif

#ifndef _EH_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#ifndef dp_iohandler_h
#   include <Dispatch/iohandler.h>
#endif

#ifndef _StrChunk_H_
#   include <StrMod/StrChunk.h>
#endif

#define _SockListenModule_H_

class ListeningPlug;
class SocketModuleChunk;
class SocketAddress;
class RWCString;

class SockListenModule : public StreamModule {
   friend class ListeningPlug;

 protected:
   inline virtual StrPlug *CreatePlug(int side);
   inline SocketModule *MakeSocketModule(int fd, SocketAddress *peer);

   int sockfd, last_error;
   ListeningPlug *lplug;
   unsigned char plug_pulled;
   SocketModule *newmodule;
   SocketAddress *myaddr;

 public:
   inline virtual int CanCreate(int side) const;
   inline ListeningPlug *MakePlug(int side = 1);
   inline virtual int OwnsPlug(StrPlug *plug) const;
   inline virtual int DeletePlug(StrPlug *plug);

   virtual int ModuleType() const               { return(SockListenModuleNum); }
   virtual int AreYouA(int mtype) const         {
      return((mtype == SockListenModuleNum) || StreamModule::AreYouA(mtype));
   }

   int HasError() const                         { return(last_error != 0); }
   int ErrorNum() const                         { return(last_error); }
   void ClearError()                            { last_error = 0; }
   RWCString ErrorString() const;
   const SocketAddress *GetBoundAddress() const { return(myaddr); }

   SockListenModule(const SocketAddress &bind_addr, int qlen = 1);
   virtual ~SockListenModule();
};


class ListeningPlug : public StrPlug, public IOHandler {
   SockListenModule *parent;

   virtual StreamModule *ParentModule() const   { return(parent); }
   virtual StrChunk *InternalRead();

   virtual void WriteableNotify(); // Don't care if the other plug can be read
                                  // because I can't be written to.

   virtual int inputReady(int fd);
// virtual int outputReady(int fd);      // We will really only care about the
// virtual int exceptionRaised(int fd);  // inputReady condition. If any of the
// virtual void timerExpired(long, usec);// other ones are triggered, we'll
                                         // trust the default implementation to
 public:                                 // do the right thing.
   virtual int PlugType() const                 { return(ListeningPlugNum); }
   virtual int AreYouA(int ptype) const         {
      return((ptype == ListeningPlugNum) || StrPlug::AreYouA(ptype));
   }

   virtual int CanWrite() const                 { return(0); }
   virtual int Write(StrChunk *);  // Calling this will cause unpredictable
                                   // things to happen.
   inline virtual int CanRead() const;
   inline SocketModuleChunk *Read();

   inline SockListenModule *ModuleFrom() const;
   virtual int Side() const                     { return(0); }

   ListeningPlug(SockListenModule *p);
   virtual ~ListeningPlug();
};


// This class is sort of a fake class to shove the listening socket into the
// standard StreamModule framework. The SockListenModule mints SocketModule's,
// but all the plug definitions require that StrChunk's be passed between the
// plugs. This class wraps a SocketModule in a StrChunk. SocketModule's really
// can't be passed to anywhere outside the process, so all the methods for this
// class are going to act like this SockModuleChunk is empty.

class SocketModuleChunk : public StrChunk {
   static int junk;   // Something for GetCVoidP() to return a pointer to.

 protected:
   SocketModule *module;

 public:
   virtual unsigned int Length() const                            { return(0); }
   inline virtual unsigned int ChunkType() const;
   inline virtual int IsChunkType(unsigned int ctype) const;

   virtual int FillFromFd(int fd, int start = 0, int maxsize = 0) { return(0); }
   virtual int PutIntoFd(int fd, int start = 0, int maxsize = 0)  { return(0); }

   virtual const void *GetCVoidP()                            { return(&junk); }

   SocketModule *GetModule() const                           { return(module); }
   void ReleaseModule()                                          { module = 0; }

   SocketModuleChunk(SocketModule *mod) : module(mod)                         {}
   inline virtual ~SocketModuleChunk();
};
   

// ------------------SockListenModule inline functions--------------------------
inline StrPlug *SockListenModule::CreatePlug(int side)
{
   if (lplug == 0 || side == 0 || plug_pulled)
      return(0);
   else {
      plug_pulled = 1;
      return(lplug);
   }
}

inline SocketModule *SockListenModule::MakeSocketModule(int fd,
							SocketAddress *peer)
{
   return (new SocketModule(fd, peer));
}

inline virtual int SockListenModule::CanCreate(int side) const
{
   return((side == 0) && !plug_pulled);
}

inline ListeningPlug *SockListenModule::MakePlug(int side = 1)
{
   return((ListeningPlug *)(CreatePlug(side)));
}

inline int SockListenModule::OwnsPlug(StrPlug *plug) const
{
   return(plug_pulled && (plug == lplug));
}

inline int SockListenModule::DeletePlug(StrPlug *plug)
{
   if (OwnsPlug(plug)) {
      plug_pulled = 0;
      return(1);
   } else
      return(0);
}

//--------------------------ListeningPlug inline functions----------------------
inline int ListeningPlug::CanRead() const
{
   return(ModuleFrom()->plug_pulled && (ModuleFrom()->newmodule != 0));
}

inline SocketModuleChunk *ListeningPlug::Read()
{
   return((SocketModuleChunk *)InternalRead());
}

inline SockListenModule *ListeningPlug::ModuleFrom() const
{
   return((SockListenModule *)ParentModule());
}

//-------------------SocketModuleChunk inline functions-------------------------
inline virtual unsigned int SocketModuleChunk::ChunkType() const
{
   return(EHChunk::SocketModuleChunk);
}

inline virtual int SocketModuleChunk::IsChunkType(unsigned int ctype) const
{
   return((ctype == EHChunk::SocketModuleChunk) ||
	  StrChunk::IsChunkType(ctype));
}

inline SocketModuleChunk::~SocketModuleChunk()
{
   if (module)
      delete module;
}

#endif
