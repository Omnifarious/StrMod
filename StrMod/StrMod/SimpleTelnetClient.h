#ifndef _STR_SimpleTelnet_H_  // -*-c++-*-

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

#include <StrMod/StreamModule.h>
#include <StrMod/STR_ClassIdent.h>

#define _STR_SimpleTelnet_H_

namespace strmod {
namespace strmod {

/** \class SimpleTelnetClient SimpleTelnetClient.h StrMod/SimpleTelnetClient.h
 * \brief Only will handle the case when the server will do supress go ahead,
 * and echo.
 *
 * This is a very simple telnet protocol handler.  It won't support ANY options
 * besides the server doing supress go ahead, and echo, and it requires the
 * server to support and enable those options.  This means that it will only do
 * 'character at a time' mode.
 */
class SimpleTelnetClient : public StreamModule {
   class UPlug;
   class SPlug;
   friend class UPlug;
   friend class SPlug;
 public:
   enum Sides { ToServer, ToUser };
   static const STR_ClassIdent identifier;

   SimpleTelnetClient();
   ~SimpleTelnetClient();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool canCreate(int side) const;
   inline virtual bool ownsPlug(const Plug *plug) const;
   virtual bool deletePlug(Plug *plug);

   /** Resets the states of the sent_do_supga_ and sent_do_echo_ flags.
    * Resetting these flags has the effect of causing the client to attempt to
    * renegotiate these options
    */
   void reset();

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual Plug *i_MakePlug(int side);

   //: Handles sending telnet protocol messages for initial negotiation
   bool doProtocol();
   //: Checks the state of things and updates all the readable and writeable
   //: flags.
   void updatePlugFlags();

   //: Called when a read is done from the user plug.
   const StrChunkPtr userRead();

   //: Called when a write is done on the server plug.
   // <p>This handles properly replying to the messages the server sends.</p>
   void serverWrite(const StrChunkPtr &ptr);

 private:
   class UPlug : public Plug {
    public:
      friend class SimpleTelnetClient;
		friend class SPlug;
      
      static const STR_ClassIdent identifier;

      UPlug(SimpleTelnetClient &parent) : Plug(parent)  { }
      ~UPlug()                                          { }

      inline virtual int AreYouA(const ClassIdent &cid) const;

      inline SimpleTelnetClient &getParent() const;
      virtual int side() const                          { return(ToUser); }

    protected:
      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      virtual bool needsNotifyReadable() const          { return(true); }
      inline virtual void otherIsReadable();

      inline virtual const StrChunkPtr i_Read();
      inline virtual void i_Write(const StrChunkPtr &chnk);

      inline bool canReadOther() const;
   };

   class SPlug : public Plug {
    public:
      friend class SimpleTelnetClient;
		friend class UPlug;
      
      static const STR_ClassIdent identifier;

      SPlug(SimpleTelnetClient &parent) : Plug(parent)  { }
      ~SPlug()                                          { }

      inline virtual int AreYouA(const ClassIdent &cid) const;

      inline SimpleTelnetClient &getParent() const;
      virtual int side() const                          { return(ToServer); }

    protected:
      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      virtual bool needsNotifyWriteable() const         { return(true); }
      inline virtual void otherIsWriteable();

      inline virtual const StrChunkPtr i_Read();
      inline virtual void i_Write(const StrChunkPtr &chnk);

      inline bool canWriteOther() const;
   };

 private:
   bool sent_do_supga_;
   bool sent_do_echo_;
   StrChunkPtr toserver_;
   StrChunkPtr touser_;
   bool uplugcreated_;
   UPlug userplug_;
   bool splugcreated_;
   SPlug serverplug_;
};

//------------------------------inline function--------------------------------

inline int SimpleTelnetClient::AreYouA(const ClassIdent &cid) const
{
   return((cid == identifier) || StreamModule::AreYouA(cid));
}

inline bool SimpleTelnetClient::canCreate(int side) const
{
   return(((side == ToServer) && !splugcreated_) ||
	  ((side == ToUser)   && !uplugcreated_));
}

inline  bool SimpleTelnetClient::ownsPlug(const Plug *plug) const
{
   return((splugcreated_ && (plug == &serverplug_)) ||
	  (uplugcreated_ && (plug == &userplug_)));
}

//--

inline int SimpleTelnetClient::UPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline SimpleTelnetClient &SimpleTelnetClient::UPlug::getParent() const
{
   return(static_cast<SimpleTelnetClient &>(Plug::getParent()));
}

inline void SimpleTelnetClient::UPlug::otherIsReadable()
{
   getParent().updatePlugFlags();
}

inline const StrChunkPtr SimpleTelnetClient::UPlug::i_Read()
{
   return(getParent().userRead());
}

inline bool SimpleTelnetClient::UPlug::canReadOther() const
{
   Plug *other = pluggedInto();
   return((other == NULL) ? false : getFlagsFrom(*other).canread_);
}

//--

inline int SimpleTelnetClient::SPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline SimpleTelnetClient &SimpleTelnetClient::SPlug::getParent() const
{
   return(static_cast<SimpleTelnetClient &>(Plug::getParent()));
}

inline void SimpleTelnetClient::SPlug::otherIsWriteable()
{
   getParent().updatePlugFlags();
}

inline void SimpleTelnetClient::SPlug::i_Write(const StrChunkPtr &chnk)
{
   getParent().serverWrite(chnk);
}

inline bool SimpleTelnetClient::SPlug::canWriteOther() const
{
   Plug *other = pluggedInto();
   return((other == NULL) ? false : getFlagsFrom(*other).canwrite_);
}

};  // namespace strmod
};  // namespace strmod

#endif
