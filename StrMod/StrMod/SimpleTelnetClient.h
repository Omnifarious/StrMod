#ifndef _STR_SimpleTelnet_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/StreamModule.h>

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

   SimpleTelnetClient();
   ~SimpleTelnetClient();

   inline bool canCreate(int side) const override;
   inline bool ownsPlug(const Plug *plug) const override;
   bool deletePlug(Plug *plug) override;

   /** Resets the states of the sent_do_supga_ and sent_do_echo_ flags.
    * Resetting these flags has the effect of causing the client to attempt to
    * renegotiate these options
    */
   void reset();

 protected:
   Plug *i_MakePlug(int side) override;

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

      UPlug(SimpleTelnetClient &parent) : Plug(parent)  { }
      ~UPlug() = default;

      inline SimpleTelnetClient &getParent() const;
      int side() const override                         { return(ToUser); }

    protected:
      bool needsNotifyReadable() const override         { return(true); }
      inline void otherIsReadable() override;

      inline const StrChunkPtr i_Read() override;
      inline void i_Write(const StrChunkPtr &chnk) override;

      inline bool canReadOther() const;
   };

   class SPlug : public Plug {
    public:
      friend class SimpleTelnetClient;
		friend class UPlug;

      SPlug(SimpleTelnetClient &parent) : Plug(parent)  { }
      ~SPlug() = default;

      inline SimpleTelnetClient &getParent() const;
      int side() const override                         { return(ToServer); }

    protected:
      bool needsNotifyWriteable() const override        { return(true); }
      inline void otherIsWriteable() override;

      inline const StrChunkPtr i_Read() override;
      inline void i_Write(const StrChunkPtr &chnk) override;

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

}  // namespace strmod
}  // namespace strmod

#endif
