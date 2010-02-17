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

/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "SimpleTelnetClient.h"
#endif

#include "StrMod/SimpleTelnetClient.h"
#include "StrMod/TelnetParserData.h"

const STR_ClassIdent SimpleTelnetClient::identifier(43UL);
const STR_ClassIdent SimpleTelnetClient::UPlug::identifier(44UL);
const STR_ClassIdent SimpleTelnetClient::SPlug::identifier(45UL);

SimpleTelnetClient::SimpleTelnetClient()
     : sent_do_supga_(false), sent_do_echo_(false),
       uplugcreated_(false), userplug_(*this),
       splugcreated_(false), serverplug_(*this)
{
   reset();
}

SimpleTelnetClient::~SimpleTelnetClient()
{
}

bool SimpleTelnetClient::deletePlug(Plug *plug)
{
   if ((plug == &userplug_) && uplugcreated_)
   {
      uplugcreated_ = false;
      userplug_.unPlug();
      return(true);
   }
   else if ((plug == &serverplug_) && splugcreated_)
   {
      splugcreated_ = false;
      serverplug_.unPlug();
      return(true);
   }
   return(false);
}

void SimpleTelnetClient::reset()
{
   sent_do_supga_ = false;
   sent_do_echo_ = false;
   if (doProtocol())
   {
      updatePlugFlags();
   }
}

StreamModule::Plug *SimpleTelnetClient::i_MakePlug(int side)
{
   assert(((side == ToServer) && !splugcreated_) ||
	  ((side == ToUser) && !uplugcreated_));

   if (side == ToServer)
   {
      splugcreated_ = true;
      return(&serverplug_);
   }
   else
   {
      uplugcreated_ = true;
      return(&userplug_);
   }
}

bool SimpleTelnetClient::doProtocol()
{
   const U1Byte echoopt = 1;
   const U1Byte supgaopt = 3;

   if ((!sent_do_supga_ || !sent_do_echo_) && !toserver_)
   {
      U1Byte sendopt = echoopt;
      if (!sent_do_supga_)
      {
	 sendopt = supgaopt;
	 sent_do_supga_ = true;
      }
      else
      {
	 sent_do_echo_ = true;
      }

      toserver_ = new TelnetParser::OptionNegotiation(TelnetParser::OptionNegotiation::DO, sendopt);
      return(true);
   }
   return(false);
}

void SimpleTelnetClient::updatePlugFlags()
{
   setReadableFlagFor(&serverplug_,
		      (toserver_ || (uplugcreated_
				     && userplug_.canReadOther())));
   setWriteableFlagFor(&userplug_,
		       (!toserver_ && splugcreated_
			&& serverplug_.canWriteOther()));

   setReadableFlagFor(&userplug_, touser_);
   setWriteableFlagFor(&serverplug_, (!touser_ && !toserver_));
}

const StrChunkPtr SimpleTelnetClient::userRead()
{
   StrChunkPtr tmp = touser_;
   touser_.ReleasePtr();
   updatePlugFlags();
   return(tmp);
}

void SimpleTelnetClient::serverWrite(const StrChunkPtr &ptr)
{
   assert(!toserver_);
   if (ptr->AreYouA(TelnetParser::TelnetData::identifier))
   {
      if (ptr->AreYouA(TelnetParser::OptionNegotiation::identifier))
      {
	 StrChunkPtrT<TelnetParser::OptionNegotiation> optneg =
	    static_cast<TelnetParser::OptionNegotiation *>(ptr.GetPtr());

	 if (optneg->getRequest() == TelnetParser::OptionNegotiation::WILL)
	 {
	    if ((optneg->getType() != 1) && (optneg->getType() != 3))
	    {
	       toserver_ = new TelnetParser::OptionNegotiation(
		  TelnetParser::OptionNegotiation::DONT, optneg->getType());
	    }
	 }
	 else if (optneg->getRequest() == TelnetParser::OptionNegotiation::DO)
	 {
	    if (optneg->getType() == 3)
	    {
	       toserver_ = new TelnetParser::OptionNegotiation(
		  TelnetParser::OptionNegotiation::WILL, 3);
	    }
	    else
	    {
	       toserver_ = new TelnetParser::OptionNegotiation(
		  TelnetParser::OptionNegotiation::WONT, optneg->getType());
	    }
	 }
	 else if (optneg->getRequest() == TelnetParser::OptionNegotiation::DONT)
	 {
	    toserver_ = new TelnetParser::OptionNegotiation(
	       TelnetParser::OptionNegotiation::WONT, optneg->getType());
	 }
	 // WONTs are always ignored.  The rule about no automated retries of
	 // failed negotiations prevents us from resending a DO if the server
	 // refuses SUPRESS GOAHREAD or ECHO.  We'll just pretend it didn't
	 // because it doesn't really matter in terms of how we process
	 // things.
      }
      // Anything other than option negotiation is also ignored.
   }
   else
   {
      touser_ = ptr;
   }
   updatePlugFlags();
}

const StrChunkPtr SimpleTelnetClient::SPlug::i_Read()
{
   SimpleTelnetClient &parent = getParent();
   if (parent.toserver_)
   {
      StrChunkPtr tmp = parent.toserver_;
      parent.toserver_.ReleasePtr();
      if (!parent.doProtocol())  // if doProtocol didn't put more in toserver_
      {
	 parent.updatePlugFlags();
      }
      return(tmp);
   }
   else
   {
      assert(parent.uplugcreated_);
      UPlug &sibling = parent.userplug_;

      assert(sibling.pluggedInto());
      Plug &readfrom = *(sibling.pluggedInto());

      assert(readfrom.isReadable());

      setIsReading(readfrom, true);
      StrChunkPtr tmp = sibling.readOther();
      setIsReading(readfrom, false);
      return(tmp);
   }
}

void SimpleTelnetClient::UPlug::i_Write(const StrChunkPtr &ptr)
{
   SimpleTelnetClient &parent = getParent();
   assert(!parent.toserver_ && parent.splugcreated_
	  && parent.serverplug_.canWriteOther());
   assert(isWriteable());

   assert(parent.serverplug_.pluggedInto());
   Plug &writeto = *(parent.serverplug_.pluggedInto());

   if (getFlagsFrom(writeto).iswriting_)
   {
      parent.toserver_ = ptr;
      setWriteable(false);
   }
   else
   {
      setIsWriting(writeto, true);
      parent.serverplug_.writeOther(ptr);
      setIsWriting(writeto, false);
   }
}
