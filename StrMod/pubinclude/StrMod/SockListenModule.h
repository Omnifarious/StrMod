#ifndef _STR_SockListenModule_H_  // -*-c++-*-

/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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


/* $Header$ */

// For a log, see ../ChangeLog

// $Revision$

#include <string>
#include <memory>

#include <UniEvent/EventPtr.h>
#include <UniEvent/UnixEventRegistry.h>
#include <UniEvent/UNIXError.h>
#include <EHnet++/SocketAddress.h>

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#ifndef _STR_StrChunk_H_
#  include <StrMod/StrChunk.h>
#endif

#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif

#ifndef _STR_SocketModule_H_
#  include <StrMod/SocketModule.h>
#endif


#define _STR_SockListenModule_H_

namespace strmod {
namespace strmod {

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
class SocketModuleChunk : public StrChunk
{
 public:
   //! Creates a SocketModule chunk wrapping SocketModule 'mod'.
   explicit SocketModuleChunk(SocketModule *mod) : module_(mod)  { }
   //! If 'ReleaseModule' hasn't been called, also deletes wrapped SocketModule
   inline virtual ~SocketModuleChunk();

   unsigned int Length() const override                 { return 0; }

   /** Returns the wrapped SocketModule, and possibly forget about its existence.
    * @param release If this parameter is true, the SocketModuleChunk forgets
    * the wrapped SocketModule and you become responsible for managing its
    * existence (ie you must delete it at the proper time).
    */
   SocketModule *getModule(bool release = true)
   {
      SocketModule *ret = module_;
      if (release)
      {
         module_ = 0;
      }
      return ret;
   }

 protected:
   //! Accept a ChunkVisitor, and maybe lead it through your children.
   void acceptVisitor(ChunkVisitor &) override
   {
   }

 private:
   SocketModule *module_;
};

typedef ::std::shared_ptr<SocketModuleChunk> SocketModuleChunkPtr;

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
   friend class FDPollRdEv;
   class FDPollErEv;
   friend class FDPollErEv;

 public:
   class SLPlug;  // Declared at end of public section for clarity.
   friend class SLPlug;

   /** \brief What address am I going to listen on, and what's the length of
    * the pending connection queue?
    */
   SockListenModule(const ehnet::SocketAddress &bind_addr,
                    unievent::Dispatcher &disp,
                    unievent::UnixEventRegistry &ureg,
		    int qlen = 1);
   //! Closes the socket being listened to.
   virtual ~SockListenModule();

   inline bool canCreate(int side = 0) const override;
   inline SLPlug *makePlug(int side = 0);
   inline bool ownsPlug(const Plug *plug) const override;
   inline bool deletePlug(Plug *plug) override;

   //! Has there been an error of any kind?
   bool hasError() const noexcept               { return has_error_; }
   //! What was the error, if any?
   const unievent::UNIXError &getError() const noexcept;
   //! Pretend no error happened.
   void clearError() noexcept;

   //! What's the local address for this socket?
   const ehnet::SocketAddress &GetBoundAddress() const { return(myaddr_); }

   /** \class SLPlug SockListenModule.h StrMod/SockListenModule.h
    * \brief A plug from a SockListenModule.
    */
   class SLPlug : public StreamModule::Plug {
      friend class SockListenModule;

    public:
      inline SockListenModule &getParent() const;
      int side() const override                          { return(0); }

      //! Read a socket module chunk, if there is one.
      const SocketModuleChunkPtr getConnection();

    protected:
      //! Note that this can ONLY be constructed using a SockListenModule
      SLPlug(SockListenModule &parent) : Plug(parent)   { }
      /** \brief Protected because mere mortals shouldn't just create and
       * destroy these things at random.  */
      virtual ~SLPlug() = default;

      //! Forwards to getParent()->plugRead()
      const StrChunkPtr i_Read() override;
      /** \brief A dead operation that either assert fails, or is a no-op that
       * throws away its data. */
      void i_Write(const StrChunkPtr &ptr) override;
   };

 protected:
   inline Plug *i_MakePlug(int side) override;

   /** Make a socket module once I've done the accept.
    * Note, ownership of <code>peer</code> is being passed here.
    */
   inline SocketModule *makeSocketModule(int fd, ehnet::SocketAddress *peer,
					 unievent::Dispatcher &disp,
                                         unievent::UnixEventRegistry &ureg);

   //! Set an error so that hasError and getError return something.
   inline void setError(const unievent::UNIXError &err) noexcept;

   //! Return the new module (if any) and try to 'accept' another connection.
   SocketModule *getNewModule();

 private:
   typedef unievent::Dispatcher Dispatcher;
   typedef unievent::UnixEventRegistry UnixEventRegistry;

   int sockfd_;
   unsigned char errorstore_[sizeof(unievent::UNIXError)];
   bool has_error_;
   SLPlug lplug_;
   bool plug_pulled_;
   bool checking_read_;
   SocketModule *newmodule_;
   ehnet::SocketAddress &myaddr_;
   Dispatcher &disp_;
   UnixEventRegistry &ureg_;
   ::std::shared_ptr<FDPollEv> readev_;
   ::std::shared_ptr<FDPollEv> errorev_;

   void eventRead();
   void eventError();
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

// ---------------------SockListenModule inline functions----------------------

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
SockListenModule::makeSocketModule(int fd, ehnet::SocketAddress *peer,
                                   unievent::Dispatcher &disp,
                                   unievent::UnixEventRegistry &ureg)
{
   // Ownership of peer is passed here.
   return(new SocketModule(fd, peer, disp, ureg));
}

//-----------------SockListenModule::SLPlug inline functions-------------------

inline SockListenModule &SockListenModule::SLPlug::getParent() const
{
   return(static_cast<SockListenModule &>(Plug::getParent()));
}

}  // namespace strmod
}  // namespace strmod

#endif
