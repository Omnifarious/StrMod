#ifndef _STR_SockListenModule_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
// Revision 0.12  1995/04/18  03:39:29  hopper
// Merged revisions 0.11 and 0.11.0.3
//
// Revision 0.11.0.3  1994/07/18  04:00:09  hopper
// Fixed minor syntax error that gcc 2.5.8 didn't catch, but 2.6.0 did.
//
// Revision 0.11.0.2  1994/06/16  02:40:45  hopper
// Implemented changes neccessary to work with new StrChunk class.
// Added RCS Revision thing.
// Added #pragma interfeace for GNU stuff.
//
// Revision 0.11.0.1  1994/05/16  01:50:01  hopper
// Changed String to RWCString to work with Rogue Wave classes.
//
// Revision 0.11  1994/05/07  03:40:55  hopper
// Move enclosing #ifndef ... #define bits around. Changed names of some
// non-multiple include constants. Changed directories for certain
// type of include files.
//
// Revision 0.10  1992/05/17  22:40:28  hopper
// Genesis!
//

// $Revision$

#ifndef NO_RcsID
static char _SockListenModule_H_rcsID[] =
      "$Id$";
#endif

#ifndef _STR_SocketModule_H_
#  include <StrMod/SocketModule.h>
#endif

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#include <Dispatch/iohandler.h>

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif

#define _STR_SockListenModule_H_

class ListeningPlug;
class SocketModuleChunk;
class SocketAddress;
class RWCString;

class SockListenModule : public StreamModule {
   friend class ListeningPlug;

 public:
   static const STR_ClassIdent identifier;

   SockListenModule(const SocketAddress &bind_addr, int qlen = 1);
   virtual ~SockListenModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanCreate(int side = 1) const;
   inline ListeningPlug *MakePlug(int side = 1);
   inline virtual bool OwnsPlug(StrPlug *plug) const;
   inline virtual bool DeletePlug(StrPlug *plug);

   bool HasError() const                           { return(last_error != 0); }
   int ErrorNum() const                            { return(last_error); }
   void ClearError()                               { last_error = 0; }
   RWCString ErrorString() const;
   const SocketAddress *GetBoundAddress() const    { return(myaddr); }

 protected:
   virtual const ClassIdent *i_GetIdent() const    { return(&identifier); }

   inline virtual StrPlug *CreatePlug(int side);
   inline SocketModule *MakeSocketModule(int fd, SocketAddress *peer);

   int sockfd, last_error;
   ListeningPlug *lplug;
   bool plug_pulled;
   SocketModule *newmodule;
   SocketAddress *myaddr;
};

//----------------------------ListeningPlug class------------------------------

class ListeningPlug : public StrPlug, public IOHandler {
 public:
   static const STR_ClassIdent identifier;

   ListeningPlug(SockListenModule *p);
   virtual ~ListeningPlug();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual bool CanWrite() const                       { return(false); }
   virtual bool Write(StrChunk *);  // Calling this will cause unpredictable
                                    // things to happen.
   inline virtual bool CanRead() const;
   inline SocketModuleChunk *Read();

   inline SockListenModule *ModuleFrom() const;
   virtual int Side() const                            { return(1); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   SockListenModule *parent;

   virtual StreamModule *ParentModule() const          { return(parent); }
   virtual StrChunk *InternalRead();

   virtual void WriteableNotify();
// virtual void ReadableNotify(); // Don't care if the other plug can be read
                                  // because I can't be written to.

   virtual int inputReady(int fd);
// virtual int outputReady(int fd);      // We will really only care about the
// virtual int exceptionRaised(int fd);  // inputReady condition. If any of the
// virtual void timerExpired(long, usec);// other ones are triggered, we'll
                                         // trust the default implementation to
};                                       // do the right thing.

//--------------------------SocketModuleChunk class----------------------------

// This class is sort of a fake class to shove the listening socket into the
// standard StreamModule framework. The SockListenModule mints SocketModule's,
// but all the plug definitions require that StrChunk's be passed between the
// plugs. This class wraps a SocketModule in a StrChunk. SocketModule's really
// can't be passed to anywhere outside the process, so all the methods for this
// class are going to act like this SockModuleChunk is empty.

class SocketModuleChunk : public StrChunk {
   static int junk;   // Something for GetCVoidP() to return a pointer to.

 public:
   static const STR_ClassIdent identifier;

   SocketModuleChunk(SocketModule *mod) : module(mod)  { }
   inline virtual ~SocketModuleChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                 { return(0); }

   virtual const void *GetCVoidP()                     { return(&junk); }

   SocketModule *GetModule() const                     { return(module); }
   void ReleaseModule()                                { module = 0; }

 protected:
   SocketModule *module;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual int i_FillFromFd(int fd, int start,
				   int maxsize, int issocket, int flags);
   inline virtual int i_PutIntoFd(int fd, int start,
				  int maxsize, int issocket, int flags);
};
   

// ---------------------SockListenModule inline functions----------------------

inline int SockListenModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline virtual bool SockListenModule::CanCreate(int side) const
{
   return((side == 1) && !plug_pulled);
}

inline ListeningPlug *SockListenModule::MakePlug(int side)
{
   return((ListeningPlug *)(CreatePlug(side)));
}

inline bool SockListenModule::OwnsPlug(StrPlug *plug) const
{
   return(plug_pulled && (plug == lplug));
}

inline bool SockListenModule::DeletePlug(StrPlug *plug)
{
   if (OwnsPlug(plug)) {
      plug_pulled = false;
      return(true);
   } else
      return(false);
}

inline StrPlug *SockListenModule::CreatePlug(int side)
{
   if (lplug == 0 || side == 0 || plug_pulled)
      return(0);
   else {
      plug_pulled = true;
      return(lplug);
   }
}

inline SocketModule *SockListenModule::MakeSocketModule(int fd,
							SocketAddress *peer)
{
   return (new SocketModule(fd, peer));
}

//----------------------ListeningPlug inline functions-------------------------

inline int ListeningPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrPlug::AreYouA(cid));
}

inline bool ListeningPlug::CanRead() const
{
   SockListenModule *parent = ModuleFrom();

   return(parent->plug_pulled && (parent->newmodule != 0));
}

inline SocketModuleChunk *ListeningPlug::Read()
{
   return((SocketModuleChunk *)InternalRead());
}

inline SockListenModule *ListeningPlug::ModuleFrom() const
{
   return((SockListenModule *)ParentModule());
}

//--------------------SocketModuleChunk inline functions-----------------------

inline SocketModuleChunk::~SocketModuleChunk()
{
   if (module)
      delete module;
}

inline int SocketModuleChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline int SocketModuleChunk::i_FillFromFd(int fd, int start,
					    int maxsize, int issocket,
					    int flags)
{
   return(0);
}

inline int SocketModuleChunk::i_PutIntoFd(int fd, int start,
					  int maxsize, int issocket,
					  int flags)
{
   return(0);
}

#endif
