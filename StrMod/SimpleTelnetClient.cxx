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

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "SimpleTelnetClient.h"
#endif

#include "StrMod/SimpleTelnetClient.h"
#include "StrMod/TelnetChunkerData.h"
#include "StrMod/TelnetChars.h"
#include <tr1/memory>
#include <cassert>

namespace strmod {
namespace strmod {

using lcore::U1Byte;

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

      toserver_ = StrChunkPtr(new TelnetChunker::OptionNegotiation(TelnetChars::O_DO, sendopt));
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
   touser_.reset();
   updatePlugFlags();
   return(tmp);
}

void SimpleTelnetClient::serverWrite(const StrChunkPtr &ptr)
{
   typedef TelnetChunker::TelnetData tdata_t;
   typedef TelnetChunker::OptionNegotiation topt_t;
   using ::std::tr1::shared_ptr;
   using ::std::tr1::dynamic_pointer_cast;
   typedef shared_ptr<tdata_t> tdptr_t;
   typedef shared_ptr<topt_t> toptr_t;

   assert(!toserver_);

   if (tdptr_t dptr = dynamic_pointer_cast<tdata_t, StrChunk>(ptr))
   {
      if (toptr_t optneg = dynamic_pointer_cast<topt_t, tdata_t>(dptr))
      {
	 if (optneg->getRequest() == TelnetChars::O_WILL)
	 {
	    if ((optneg->getType() != 1) && (optneg->getType() != 3))
	    {
	       toserver_.reset(new TelnetChunker::OptionNegotiation(
                                  TelnetChars::O_DONT, optneg->getType()
                                  ));
	    }
	 }
	 else if (optneg->getRequest() == TelnetChars::O_DO)
	 {
	    if (optneg->getType() == 3)
	    {
	       toserver_.reset(new TelnetChunker::OptionNegotiation(
                                  TelnetChars::O_WILL, 3
                                  ));
	    }
	    else
	    {
	       toserver_.reset(new TelnetChunker::OptionNegotiation(
                                  TelnetChars::O_WONT, optneg->getType()
                                  ));
	    }
	 }
	 else if (optneg->getRequest() == TelnetChars::O_DONT)
	 {
	    toserver_.reset(new TelnetChunker::OptionNegotiation(
                               TelnetChars::O_WONT, optneg->getType()
                               ));
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
      parent.toserver_.reset();
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
   assert(getFlagsFrom(*this).canwrite_);

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

}  // End namespace strmod
}  // End namespace strmod
