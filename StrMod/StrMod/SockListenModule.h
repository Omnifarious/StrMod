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

#include <string>

#include <UniEvent/EventPtrT.h>
#include <UniEvent/UNIXpollManager.h>
#include <UniEvent/UNIXError.h>

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif

#ifndef _STR_StrChunkPtrT_H_
#  include <StrMod/StrChunkPtrT.h>
#endif

#ifndef _STR_SocketModule_H_
#  include <StrMod/SocketModule.h>
#endif


#define _STR_SockListenModule_H_

class SocketAddress;

namespace strmod {
namespace strmod {

class ListeningPlug;
class SocketModuleChunk;

/** \class SocketModuleChunk SockListenModule.h StrMod/SockListenModule.h
 * \brief A special 'zero length' chunk that contains a SocketModule.
 *
 * This class is sort of a fake class to shove the listening socket into the
 * standard StreamModule framework.  The SockListenModule mints SocketModule
 * instances, but all the plug definitions require that instances of StrChunk be
 * passed between the plugs.  This class wraps a SocketModule in a StrChunk.
 * SocketModule's really can't be passed to anywhere outside the process, so all
 * the methods for this class are going to act like this SockModuleChunk is
 * empty.
 */
class SocketModuleChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   //! Creates a SocketModule chunk wrapping SocketModule 'mod'.
   explicit SocketModuleChunk(SocketModule *mod) : module_(mod)  { }
   //! If 'ReleaseModule' hasn't been called, also deletes wrapped SocketModule
   inline virtual ~SocketModuleChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                 { return 0; }

   //! Returns the wrapped SocketModule
   SocketModule *GetModule() const                     { return module_; }
   //! Tells the SocketModuleChunk it is no longer responsible for the module it wraps.
   void ReleaseModule()                                { module_ = 0; }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   //! Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation)             { }

 private:
   SocketModule *module_;
};

typedef StrChunkPtrT<SocketModuleChunk> SocketModuleChunkPtr;

//--------------------------SockListenModule class-----------------------------

/** \class SockListenModule SockListenModule.h StrMod/SockListenModule.h
 * \brief A class that accepts connections on a particular socket.
 *
 * This class creates instances of SocketModules and wraps them up in a
 * SocketModuleChunk.
 */
class SockListenModule : public StreamModule {
   class FDPollEv;
   friend class FDPollEv;
   class FDPollRdEv;
   class FDPollErEv;

 public:
   class SLPlug;  // Declared at end of public section for clarity.
   friend class SLPlug;
   static const STR_ClassIdent identifier;

   /** \brief What address am I going to listen on, and what's the length of
    * the pending connection queue?
    */
   SockListenModule(const SocketAddress &bind_addr,
                    unievent::Dispatcher &disp,
                    unievent::UNIXpollManager &pmgr,
		    int qlen = 1);
   //! Closes the socket being listened to.
   virtual ~SockListenModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool canCreate(int side = 0) const;
   inline SLPlug *makePlug(int side = 0);
   inline virtual bool ownsPlug(const Plug *plug) const;
   inline virtual bool deletePlug(Plug *plug);

   //! Has there been an error of any kind?
   bool hasError() const throw()               { return has_error_; }
   //! What was the error, if any?
   const unievent::UNIXError &getError() const throw();
   //! Pretend no error happened.
   void clearError() throw();

   //! What's the local address for this socket?
   const SocketAddress &GetBoundAddress() const { return(myaddr_); }

   /** \class SLPlug SockListenModule.h StrMod/SockListenModule.h
    * \brief A plug from a SockListenModule.
    */
   class SLPlug : public StreamModule::Plug {
      friend class SockListenModule;

    public:
      static const STR_ClassIdent identifier;

      inline virtual int AreYouA(const ClassIdent &cid) const;

      inline SockListenModule &getParent() const;
      virtual int side() const                          { return(0); }

      //! Read a socket module chunk, if there is one.
      const SocketModuleChunkPtr getConnection();

    protected:
      //! Note that this can ONLY be constructed using a SockListenModule
      SLPlug(SockListenModule &parent) : Plug(parent)   { }
      /** \brief Protected because mere mortals shouldn't just create and
       * destroy these things at random.  */
      virtual ~SLPlug()                                 { }

      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      //! Forwards to getParent()->plugRead()
      virtual const StrChunkPtr i_Read();
      /** \brief A dead operation that either assert fails, or is a no-op that
       * throws away its data. */
      virtual void i_Write(const StrChunkPtr &ptr);
   };

 protected:
   virtual const ClassIdent *i_GetIdent() const { return(&identifier); }

   inline virtual Plug *i_MakePlug(int side);

   /** Make a socket module once I've done the accept.
    * Note, ownership of <code>peer</code> is being passed here.
    */
   inline SocketModule *makeSocketModule(int fd, SocketAddress *peer,
					 unievent::Dispatcher &disp,
                                         unievent::UNIXpollManager &pmgr);

   //! Set an error so that hasError and getError return something.
   inline void setError(const unievent::UNIXError &err) throw();

   //! Return the new module (if any) and try to 'accept' another connection.
   SocketModule *getNewModule();

 private:
   typedef unievent::Dispatcher Dispatcher;
   typedef unievent::UNIXpollManager UNIXpollManager;

   int sockfd_;
   unsigned char errorstore_[sizeof(unievent::UNIXError)];
   bool has_error_;
   SLPlug lplug_;
   bool plug_pulled_;
   bool checking_read_;
   SocketModule *newmodule_;
   SocketAddress &myaddr_;
   Dispatcher &disp_;
   UNIXpollManager &pmgr_;
   FDPollEv *readevptr_;
   unievent::EventPtrT<UNIXpollManager::PollEvent> readev_;
   FDPollEv *errorevptr_;
   unievent::EventPtrT<UNIXpollManager::PollEvent> errorev_;

   void eventRead(unsigned int condbits);
   void eventError(unsigned int condbits);
   void doAccept();
};

//--------------------SocketModuleChunk inline functions-----------------------

inline SocketModuleChunk::~SocketModuleChunk()
{
   if (module_)
   {
      delete module_;
   }
}

inline int SocketModuleChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
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

inline SocketModule *
SockListenModule::makeSocketModule(int fd, SocketAddress *peer,
                                   unievent::Dispatcher &disp,
                                   unievent::UNIXpollManager &pmgr)
{
   // Ownership of peer is passed here.
   return(new SocketModule(fd, peer, disp, pmgr));
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

};  // namespace strmod
};  // namespace strmod

#endif
