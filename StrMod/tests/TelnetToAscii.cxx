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

// For a log, see ../Changelog

#ifdef __GNUG__
#  pragma implementation "TelnetToAscii.h"
#endif

#include "TelnetToAscii.h"
#include <StrMod/TelnetParserData.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/GroupChunk.h>
#include <cctype>
#include <cassert>

const EH_ClassIdent TelnetToAscii::identifier(
   EH_ClassNum(EH_ClassNum::User5, 0UL));

TelnetToAscii::TelnetToAscii(bool stripdata)
     : stripdata_(stripdata)
{
}

TelnetToAscii::TelnetToAscii(const string &name, bool stripdata)
     : stripdata_(stripdata)
{
   string myname = "\n";
   myname += name;
   namechunk_ = new DynamicBuffer(myname.data(), myname.size());
}

void TelnetToAscii::processIncoming()
{
   assert(incoming_ && !outgoing_ready_);
   typedef TelnetParser::TelnetData TelnetData;
   typedef TelnetParser::SingleChar SingleChar;
   typedef TelnetParser::Suboption Suboption;
   typedef TelnetParser::OptionNegotiation OptionNegotiation;

   if (!incoming_->AreYouA(TelnetData::identifier))
   {
      if (!stripdata_)
      {
	 outgoing_ = incoming_;
	 outgoing_ready_ = true;
      }
      incoming_.ReleasePtr();
   }
   else
   {
      if (incoming_->AreYouA(SingleChar::identifier))
      {
	 StrChunkPtrT<SingleChar> chnk =
	    static_cast<SingleChar *>(incoming_.GetPtr());
	 const char *asciiopt = 0;

	 switch (chnk->getType())
	 {
	  case SingleChar::TEOF:
	    asciiopt = "\nSC:<EOF>\n";
	    break;
	  case SingleChar::SUSP:
	    asciiopt = "\nSC:<SUSP>\n";
	    break;
	  case SingleChar::ABORT:
	    asciiopt = "\nSC:<ABORT>\n";
	    break;
	  case SingleChar::EOR:
	    asciiopt = "\nSC:<EOR>\n";
	    break;
	  case SingleChar::NOP:
	    asciiopt = "\nSC:<NOP>\n";
	    break;
	  case SingleChar::DM:
	    asciiopt = "\nSC:<DM>\n";
	    break;
	  case SingleChar::BRK:
	    asciiopt = "\nSC:<BRK>\n";
	    break;
	  case SingleChar::IP:
	    asciiopt = "\nSC:<IP>\n";
	    break;
	  case SingleChar::AO:
	    asciiopt = "\nSC:<AO>\n";
	    break;
	  case SingleChar::AYT:
	    asciiopt = "\nSC:<AYT>\n";
	    break;
	  case SingleChar::EC:
	    asciiopt = "\nSC:<EC>\n";
	    break;
	  case SingleChar::EL:
	    asciiopt = "\nSC:<EL>\n";
	    break;
	  case SingleChar::GA:
	    asciiopt = "\nSC:<GA>\n";
	    break;
	 }
	 DynamicBuffer *tmp = new DynamicBuffer(asciiopt, strlen(asciiopt));
	 if (namechunk_)
	 {
	    GroupChunk *chnk = new GroupChunk;
	    chnk->push_back(namechunk_);
	    chnk->push_back(tmp);
	    outgoing_ = chnk;
	 }
	 else
	 {
	    outgoing_ = tmp;
	 }
	 incoming_.ReleasePtr();
	 outgoing_ready_ = true;
      }
      else if (incoming_->AreYouA(Suboption::identifier))
      {
	 StrChunkPtrT<Suboption> subopt =
	    static_cast<Suboption *>(incoming_.GetPtr());
	 StrChunkPtrT<GroupChunk> chnk = new GroupChunk;
	 if (namechunk_)
	 {
	    chnk->push_back(namechunk_);
	 }
	 chnk->push_back(new DynamicBuffer("\nSubOption : ", 13));
	 {
	    U1Byte opt = subopt->getType();
	    char buf[5];
	    buf[0] = (opt / 100) + '0';
	    buf[1] = ((opt % 100) / 10) + '0';
	    buf[2] = (opt % 10) + '0';
	    buf[3] = ' ';
	    buf[4] = '[';
	    chnk->push_back(new DynamicBuffer(buf, 5));
	 }
	 {
	    StrChunkPtrT<BufferChunk> cooked = subopt->getCooked();
	    unsigned int len = cooked->Length();
	    unsigned char *pos = cooked->getCharP();

	    for (; len > 0; --len, ++pos)
	    {
	       char buf[10];
	       int used = 4;
	       int upper = (*pos & 0xf0) >> 4;
	       int lower = *pos & 0x0f;

	       buf[0] = '<';
	       buf[1] = (upper < 10) ? (upper + '0') : ((upper - 10) + 'A');
	       buf[2] = (lower < 10) ? (lower + '0') : ((lower - 10) + 'A');
	       buf[3] = '>';
	       if (isalnum(*pos))
	       {
		  used = 6;
		  buf[3] = ':';
		  buf[4] = *pos;
		  buf[5] = '>';
	       }
	       chnk->push_back(new DynamicBuffer(buf, used));
	    }
	 }
	 {
	    char buf[2];
	    buf[0] = ']';
	    buf[1] = '\n';
	    chnk->push_back(new DynamicBuffer(buf, 2));
	 }
	 outgoing_ = chnk;
	 incoming_.ReleasePtr();
	 outgoing_ready_ = true;
      }
      else if (incoming_->AreYouA(OptionNegotiation::identifier))
      {
	 StrChunkPtrT<OptionNegotiation> optneg =
	    static_cast<OptionNegotiation *>(incoming_.GetPtr());
	 const char *reqtype = 0;

	 switch (optneg->getRequest())
	 {
	  case OptionNegotiation::WILL:
	    reqtype = "\nWILL ";
	    break;
	  case OptionNegotiation::WONT:
	    reqtype = "\nWONT ";
	    break;
	  case OptionNegotiation::DO:
	    reqtype = "\nDO ";
	    break;
	  case OptionNegotiation::DONT:
	    reqtype = "\nDONT ";
	    break;
	 }
	 StrChunkPtrT<DynamicBuffer> chnk =
	    new DynamicBuffer(reqtype, strlen(reqtype) + 4);

	 {
	    U1Byte opt = optneg->getType();
	    unsigned char *buf = chnk->getCharP() + strlen(reqtype);
	    buf[0] = (opt / 100) + '0';
	    buf[1] = ((opt % 100) / 10) + '0';
	    buf[2] = (opt % 10) + '0';
	    buf[3] = '\n';
	 }
	 if (namechunk_)
	 {
	    GroupChunk *tmp = new GroupChunk;
	    tmp->push_back(namechunk_);
	    tmp->push_back(chnk);
	    outgoing_ = tmp;
	 }
	 else
	 {
	    outgoing_ = chnk;
	 }
	 incoming_.ReleasePtr();
	 outgoing_ready_ = true;
      }
      else
      {
	 assert(0);
      }
   }
}
