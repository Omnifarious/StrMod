#ifndef _STR_SockListenModule_H_  // -*-c++-*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

// $Revision$

#ifndef _STR_SocketModule_H_
#  include <StrMod/SocketModule.h>
#endif

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif

#ifndef _STR_StrChunkPtrT_H_
#  include <StrMod/StrChunkPtrT.h>
#endif

#include <UniEvent/EventPtrT.h>
#include <UniEvent/UNIXpollManager.h>
#include <UniEvent/UNIXError.h>

#include <string>

#define _STR_SockListenModule_H_

class ListeningPlug;
class SocketModuleChunk;
class SocketAddress;

//: A special 'zero length' chunk that contains a SocketModule.
// This class is sort of a fake class to shove the listening socket
// into the standard StreamModule framework. The SockListenModule
// mints SocketModule's, but all the plug definitions require that
// StrChunk's be passed between the plugs. This class wraps a
// SocketModule in a StrChunk. SocketModule's really can't be passed
// to anywhere outside the process, so all the methods for this class
// are going to act like this SockModuleChunk is empty.
class SocketModuleChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   SocketModuleChunk(SocketModule *mod) : module(mod)  { }
   inline virtual ~SocketModuleChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                 { return(0); }
   virtual unsigned int NumSubGroups() const           { return(0); }
   inline virtual unsigned int NumSubGroups(const LinearExtent &extent) const;
   inline virtual void FillGroupVec(GroupVector &vec,
				    unsigned int &start_index);
   inline virtual void FillGroupVec(const LinearExtent &extent,
				    GroupVector &vec,
				    unsigned int &start_index);

   SocketModule *GetModule() const                     { return(module); }
   void ReleaseModule()                                { module = 0; }

 protected:
   SocketModule *module;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }
   inline virtual void i_DropUnused(const LinearExtent &usedextent,
				    KeepDir keepdir);
};

typedef StrChunkPtrT<SocketModuleChunk> SocketModuleChunkPtr;

//--------------------------SockListenModule class-----------------------------

//: A class that accepts connections on a particular socket.
// This class creates SocketModule s and wraps them up in SocketModuleChunk s.
class SockListenModule : public StreamModule {
   class FDPollEv;
   friend class FDPollEv;
   class FDPollRdEv;
   class FDPollErEv;

 public:
   class SLPlug;  // Declared at end of public section for clarity.
   friend class SLPlug;
   static const STR_ClassIdent identifier;

   //: What address am I going to listen on, and what's the length of the
   //: pending connection queue?
   SockListenModule(const SocketAddress &bind_addr, UNIXpollManager &pmgr,
		    int qlen = 1);
   virtual ~SockListenModule();

   //: See base class
   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: See base class - Only 1 side, side 0.
   inline virtual bool canCreate(int side = 0) const;
   //: See base class - Only 1 side, side 0.
   inline SLPlug *makePlug(int side = 0);
   //: See base class
   inline virtual bool ownsPlug(const Plug *plug) const;
   //: See base class
   inline virtual bool deletePlug(Plug *plug);

   //: Has there been an error of any kind?
   bool hasError() const                       { return(last_err_ != 0); }
   //: What was the error, if any?
   const UNIXError getError() const            { return(UNIXError(last_err_)); }
   //: Pretend no error happened.
   void clearError();

   //: What's the local address for this socket?
   const SocketAddress &GetBoundAddress() const { return(myaddr_); }

   class SLPlug : public Plug {
      friend class SockListenModule;

    public:
      static const STR_ClassIdent identifier;

      //: Not that this can ONLY be constructed using a SockListenModule
      SLPlug(SockListenModule &parent) : Plug(parent)   { }
      virtual ~SLPlug()                                 { }

      //: See base class.
      inline virtual int AreYouA(const ClassIdent &cid) const;

      //: Grab Plug::getParent, and cast it to the real type of the parent.
      inline SockListenModule &getParent() const;

      //: This plug is always on side 0.
      virtual int side() const                          { return(0); }

      //: Read a socket module chunk, if there is one.
      const SocketModuleChunkPtr getConnection();

    protected:
      //: See base class.
      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      //: Forwards to getParent()->plugRead()
      virtual const StrChunkPtr i_Read();
      //: A dead operation.  It either assert fails, or is a no-op that throws
      //: away its data.
      virtual void i_Write(const StrChunkPtr &ptr);
   };

 protected:
   //: See base cass
   virtual const ClassIdent *i_GetIdent() const { return(&identifier); }

   //: See base class - The only side is 0.
   inline virtual Plug *i_MakePlug(int side);

   //: Make a socket module once I've done the accept.
   // Note, ownership of peer is being passed here.
   inline SocketModule *makeSocketModule(int fd, SocketAddress *peer,
					 UNIXpollManager &pmgr);

   SocketModule *getNewModule();

 private:
   int sockfd_, last_err_;
   SLPlug lplug_;
   bool plug_pulled_;
   bool checking_read_;
   SocketModule *newmodule_;
   SocketAddress &myaddr_;
   UNIXpollManager &pmgr_;
   FDPollEv *readevptr_;
   UNIEventPtrT<UNIXpollManager::PollEvent> readev_;
   FDPollEv *errorevptr_;
   UNIEventPtrT<UNIXpollManager::PollEvent> errorev_;

   void eventRead(unsigned int condbits);
   void eventError(unsigned int condbits);
   void doAccept();
};

//--------------------SocketModuleChunk inline functions-----------------------

inline SocketModuleChunk::~SocketModuleChunk()
{
   if (module)
   {
      delete module;
   }
}

inline int SocketModuleChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline unsigned int
SocketModuleChunk::NumSubGroups(const LinearExtent &) const
{
   return(0);
}

inline void SocketModuleChunk::FillGroupVec(GroupVector &, unsigned int &)
{
}

inline void SocketModuleChunk::FillGroupVec(const LinearExtent &,
					    GroupVector &, unsigned int &)
{
}

inline void SocketModuleChunk::i_DropUnused(const LinearExtent &, KeepDir)
{
}

// ---------------------SockListenModule inline functions----------------------

inline int SockListenModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool SockListenModule::canCreate(int side) const
{
   return((side == 0) && !plug_pulled_ && !hasError());
}

inline SockListenModule::SLPlug *SockListenModule::makePlug(int side)
{
   return((SLPlug *)(i_MakePlug(side)));
}

inline bool SockListenModule::ownsPlug(const Plug *plug) const
{
   return(plug_pulled_ && (plug == &lplug_));
}

inline bool SockListenModule::deletePlug(Plug *plug)
{
   if (ownsPlug(plug))
   {
      lplug_.unPlug();
      plug_pulled_ = false;
      return(true);
   }
   else
   {
      return(false);
   }
}

inline StreamModule::Plug *SockListenModule::i_MakePlug(int side)
{
   if (side != 0 || plug_pulled_ || hasError())
   {
      return(0);
   } else {
      plug_pulled_ = true;
      return(&lplug_);
   }
}

inline SocketModule *SockListenModule::makeSocketModule(int fd,
							SocketAddress *peer,
							UNIXpollManager &pmgr)
{
   // Ownership of peer is passed here.
   return(new SocketModule(fd, peer, pmgr));
}

//----------------------ListeningPlug inline functions-------------------------

inline int SockListenModule::SLPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline SockListenModule &SockListenModule::SLPlug::getParent() const
{
   return(static_cast<SockListenModule &>(Plug::getParent()));
}

#endif
