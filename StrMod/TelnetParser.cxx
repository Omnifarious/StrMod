/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "TelnetParser.h"
#endif

#include "StrMod/TelnetParser.h"

#ifdef __GNUG__
#  pragma implementation "TelnetParserData.h"
#endif

#include "StrMod/TelnetParserData.h"
#include "StrMod/DynamicBuffer.h"
#include "StrMod/GroupChunk.h"
#include "StrMod/StrSubChunk.h"
#include "StrMod/StrChunkPtrT.h"
#include "StrMod/StrChunkPtr.h"
#include "StrMod/GroupVector.h"
#include "StrMod/GV_Iterator.h"
#include <cstring>
#include <cassert>
#include <iostream>

const STR_ClassIdent TelnetParser::identifier(33UL);
const STR_ClassIdent TelnetParser::TelnetData::identifier(34UL);
const STR_ClassIdent TelnetParser::SingleChar::identifier(35UL);
const STR_ClassIdent TelnetParser::Suboption::identifier(36UL);
const STR_ClassIdent TelnetParser::OptionNegotiation::identifier(37UL);

const U1Byte TelnetParser::TN_IAC;
const U1Byte TelnetParser::TN_SB;
const U1Byte TelnetParser::TN_SE;
const size_t TelnetParser::subopt_size_limit_;

const U1Byte TelnetParser::Suboption::optend_[2] = {
   TelnetParser::TN_IAC, TelnetParser::TN_SE
};

TelnetParser::TelnetParser()
     : incoming_pos_(0), state_(PS_Normal), cookedsize_(0), rawsize_(0),
       negtype_(0), suboptnum_(0)
{
}

TelnetParser::~TelnetParser()
{
}

inline void TelnetParser::addToChunk(StrChunkPtrT<SuboptBuffer> &chnk,
				     size_t &cursize, U1Byte ch)
{
   assert(chnk);
   if (cursize >= chnk->Length())
   {
      size_t newsize = cursize + (cursize / 2);
      chnk->resize(newsize);
      assert(cursize < chnk->Length());
   }
   (chnk->getCharP())[cursize++] = ch;
}

TelnetParser::Actions TelnetParser::processChar(U1Byte ch)
{
   assert((state_ == PS_Normal) || (state_ == PS_Escape)
	  || (state_ == PS_SubNeg) || (state_ == PS_SuboptNum)
	  || (state_ == PS_Subopt) || (state_ == PS_SuboptEscape));

   switch (state_)
   {
    case PS_Normal:
      {
	 if (ch == TN_IAC)
	 {
	    state_ = PS_Escape;
	    return(PC_DoNothing);
	 }
	 else
	 {
	    return(PC_AddCurrentChar);
	 }
      }
      break;

    case PS_Escape:
      {
	 if (ch == TN_IAC)
	 {
	    state_ = PS_Normal;
	    return(PC_SkipAddCurrent);
	 }
	 else if (ch == TN_SB)
	 {
	    state_ = PS_SuboptNum;
	    return(PC_DoNothing);
	 }
	 else if (TelnetParser::SingleChar::isSpecial(ch))
	 {
	    state_ = PS_Normal;
	    telnetcommand_ =
	       new SingleChar(static_cast<SingleChar::Specials>(ch));
	    return(PC_AddTelnetBlock);
	 }
	 else if ((ch >= 251U) && (ch <= 254U))
	 {
	    assert((ch == OptionNegotiation::WILL)
		   || (ch == OptionNegotiation::WONT)
		   || (ch == OptionNegotiation::DO)
		   || (ch == OptionNegotiation::DONT));
	    state_ = PS_SubNeg;
	    negtype_ = ch;
	    return(PC_DoNothing);
	 }
	 else  // A silly escape came in.
	 {
	    state_ = PS_Normal;
	    // So do SOMETHING about it besides crash.
	    return(PC_AddPrevIACAndCurrentChar);
	 }
      }
      break;

    case PS_SubNeg:
      {
	 state_ = PS_Normal;
	 telnetcommand_ = new OptionNegotiation(
	    static_cast<OptionNegotiation::Requests>(negtype_),
	    ch
	    );
	 return(PC_AddTelnetBlock);
      }
      break;

    case PS_SuboptNum:
      {
	 if (ch == 255) // We don't handle the extended suboption.
	 {
	    // This shouldn't happen.  Telnet engines using this parser should
	    // always say 'WONT' and 'DONT' to the extended suboption option.
	    // Some sort of reset anyway.
	    state_ = PS_Normal;
	    // Put in weird data and hope someone notices.
	    return(PC_SkipAddCurrent);
	 }
	 else
	 {
	    suboptnum_ = ch;
	    state_ = PS_Subopt;
	    assert(cookedsize_ == 0);
	    assert(rawsize_ == 0);
	    assert(!cookedsubopt_);
	    assert(!rawsubopt_);
	    cookedsubopt_ = new SuboptBuffer;
	    cookedsubopt_->resize(16);
	    return(PC_DoNothing);
	 }
      }
      break;

    case PS_Subopt:
      {
	 if (ch == TN_IAC)
	 {
	    state_ = PS_SuboptEscape;
	 }
	 else
	 {
	    // For first few characters of overflow, generate warnings.
	    if ((cookedsize_ >= subopt_size_limit_)
		&& (cookedsize_ <= (subopt_size_limit_ + 10)))
	    {
	       cerr << __FILE__ << ":" << __LINE__
		    << " Error!  Maximum telnet suboption size of "
		    << subopt_size_limit_ << " exceeded!\n";
	    }
	    else
	    {
	       if (rawsubopt_)
	       {
		  addToChunk(rawsubopt_, rawsize_, ch);
	       }
	       addToChunk(cookedsubopt_, cookedsize_, ch);
	    }
	 }
	 return(PC_DoNothing);
      }

    case PS_SuboptEscape:
      {
	 if (ch == TN_SE)
	 {
	    cookedsubopt_->resize(cookedsize_);
	    if (rawsubopt_)
	    {
	       rawsubopt_->resize(rawsize_);
	       StrChunkPtrT<BufferChunk> cooked = cookedsubopt_.GetPtr();
	       StrChunkPtr raw = rawsubopt_;
	       telnetcommand_ = new Suboption(suboptnum_, cooked, raw);
	       cookedsubopt_.ReleasePtr();
	       cookedsize_ = 0;
	       rawsubopt_.ReleasePtr();
	       rawsize_ = 0;
	    }
	    else
	    {
	       // If there's not a raw one, that means the parser didn't
	       // detect a situation in which the 'raw' and 'cooked' version
	       // would be different, so use the same chunk for both.
	       StrChunkPtrT<BufferChunk> cooked = cookedsubopt_.GetPtr();
	       StrChunkPtr raw = cookedsubopt_;
	       telnetcommand_ = new Suboption(suboptnum_, cooked, raw);
	       cookedsubopt_.ReleasePtr();
	       cookedsize_ = 0;
	    }
	    return(PC_AddTelnetBlock);
	 }
	 else
	 {
	    state_ = PS_Subopt;
	    if ((cookedsize_ >= subopt_size_limit_)
		&& (cookedsize_ <= (subopt_size_limit_ + 10)))
	    {
	       cerr << __FILE__ << ":" << __LINE__
		    << " Error!  Maximum telnet suboption size of "
		    << subopt_size_limit_ << " exceeded!\n";
	    }
	    else
	    {
	       if (!rawsubopt_ && (ch == TN_IAC))
	       {
		  // If this is true, there'll be one less character in
		  // cookedsubopt_ than was read, so rawsubopt_ needs to be
		  // created to hold the 'raw' characters read in.
		  rawsubopt_ = new SuboptBuffer();
		  rawsubopt_->resize(cookedsubopt_->Length());
		  memcpy(rawsubopt_->getVoidP(), cookedsubopt_->getVoidP(),
			 cookedsize_);
		  rawsize_ = cookedsize_;
	       }
	       if (rawsubopt_)
	       {
		  // If we've got one, maintain it.
		  addToChunk(rawsubopt_, rawsize_, TN_IAC);
		  addToChunk(rawsubopt_, rawsize_, ch);
	       }
	       if (ch != TN_IAC)
	       {
		  addToChunk(cookedsubopt_, cookedsize_, TN_IAC);
	       }
	       addToChunk(cookedsubopt_, cookedsize_, ch);
	    }
	    return(PC_DoNothing);
	 }
      }
      break;

    default:
      assert(false);
      state_ = PS_Normal;
      return(PC_SkipAddCurrent);
      break;
   }
   // There is no return here so the compiler can catch if I've failed to
   // return in any of the cases.  One could say that I should assign a return
   // value to the variable, and return at the end, but that, in actuality, is
   // less safe because the error of forgetting to assign to the variable can
   // only be caught at run time.
}

void TelnetParser::processIncoming()
{
   assert(incoming_ && !outgoing_ready_ && !outgoing_);

   if (telnetcommand_)
   {
      assert(!outgoing_);
      outgoing_ = telnetcommand_;
      telnetcommand_.ReleasePtr();
      outgoing_ready_ = true;
      if (incoming_ && (incoming_pos_ >= incoming_->Length()))
      {
	 incoming_.ReleasePtr();
	 incoming_pos_ = 0;
      }
      return;
   }

   bool gotcommand = false;
   StrChunkPtrT<GroupChunk> multichunk;
   LinearExtent cur_region(0, incoming_->Length());
   cur_region.SubExtent_eq(LinearExtent(incoming_pos_, cur_region.Length()));
   LinearExtent sending_region(cur_region.Offset(), 0);
   LinearExtent::off_t pos = 0;

   {
      GV_Size gvsize = incoming_->NumSubGroups(cur_region);
      GroupVector gvec(gvsize);
      {
	 unsigned int tempidx = 0;
	 incoming_->FillGroupVec(cur_region, gvec, tempidx);
      }
      for (GroupVector::iterator curchar = gvec.begin();
	   !gotcommand && curchar; ++curchar, ++pos)
      {
	 const Actions action = processChar(*curchar);
	 if (action == PC_AddTelnetBlock)
	 {
	    gotcommand = true;
	 }
	 else if (action == PC_AddCurrentChar)
	 {
	    sending_region.LengthenRight(1);
	 }
	 else if (action == PC_SkipAddCurrent)
	 {
	    if (sending_region.Length() <= 0)
	    {
	       sending_region.MoveRight(pos);
	       sending_region.Length(1);
	       pos = 0;
	    }
	    else
	    {
	       if (!multichunk)
	       {
		  multichunk = new GroupChunk;
	       }
	       multichunk->push_back(new StrSubChunk(incoming_,
						     sending_region));
	       sending_region.MoveRight(pos);
	       pos = 0;
	       sending_region.Length(1);
	    }
	 }
	 else if (action == PC_AddPrevIACAndCurrentChar)
	 {
	    if (sending_region.Offset() >= 1)
	    {
	       sending_region.LengthenLeft(1);
	    }
	    else
	    {
	       if (!multichunk)
	       {
		  multichunk = new GroupChunk;
	       }
	       multichunk->push_back(new DynamicBuffer(&TN_IAC, 1));
	    }
	    sending_region.LengthenRight(1);
	 }
      }
   }

   if (!multichunk && (sending_region.Length() <= 0))
   {
      if (gotcommand)
      {
	 outgoing_ = telnetcommand_;
	 telnetcommand_.ReleasePtr();
	 outgoing_ready_ = true;
      }
   }
   else
   {
      if (multichunk)
      {
	 if (sending_region.Length() > 0)
	 {
	    multichunk->push_back(new StrSubChunk(incoming_, sending_region));
	 }
	 outgoing_ = multichunk;
	 outgoing_ready_ = true;
      }
      else
      {
	 assert(sending_region.Length() > 0);

	 if ((sending_region.Offset() == 0)
	     && (sending_region.Length() >= incoming_->Length()))
	 {
	    outgoing_ = incoming_;
	 }
	 else
	 {
	    outgoing_ = new StrSubChunk(incoming_, sending_region);
	 }
	 outgoing_ready_ = true;
      }
   }
   cur_region.SubExtent_eq(LinearExtent(pos, cur_region.Length()));
   if (!telnetcommand_ && (cur_region.Length() <= 0))
   {
      incoming_.ReleasePtr();
      incoming_pos_ = 0;
   }
   else
   {
      incoming_pos_ = cur_region.Offset();
   }
}

unsigned int
TelnetParser::SingleChar::NumSubGroups(const LinearExtent &extent) const
{
   if ((extent.Length() > 0) && (extent.Offset() < 2))
   {
      return(1);
   }
   else
   {
      return(0);
   }
}

void TelnetParser::SingleChar::FillGroupVec(const LinearExtent &extent,
					    GroupVector &vec,
					    unsigned int &start_index)
{
   if ((extent.Length() >0) && (extent.Offset() < 2))
   {
      LinearExtent me(0, 2);
      me.SubExtent_eq(extent);
      vec.SetVec(start_index++, buf_ + me.Offset(), me.Length());
   }
}

unsigned int
TelnetParser::Suboption::NumSubGroups(const LinearExtent &extent) const
{
   LinearExtent first(0, 3);
   LinearExtent second(3, 3 + rawlen_);
   LinearExtent third(3 + rawlen_, 2);
   unsigned int subgroups = 0;

   if (first.intersection(extent).Length() > 0)
   {
      ++subgroups;
   }
   if (second.intersection(extent).Length() > 0)
   {
      ++subgroups;
   }
   if (third.intersection(extent).Length() > 0)
   {
      ++subgroups;
   }
   return(subgroups);
}

void TelnetParser::Suboption::FillGroupVec(const LinearExtent &extent,
					   GroupVector &vec,
					   unsigned int &start_index)
{
   LinearExtent first(0, 3);
   LinearExtent second(3, 3 + raw_->Length());
   LinearExtent third(3 + raw_->Length(), 2);

   {
      LinearExtent temp = first.intersection(extent);

      if (temp.Length() > 0)
      {
	 vec.SetVec(start_index++, optstart_ + temp.Offset(), temp.Length());
      }
   }
   {
      LinearExtent temp = second.intersection(extent);

      if (temp.Length() > 0)
      {
	 temp.MoveLeft(second.Offset());
	 raw_->FillGroupVec(temp, vec, start_index);
      }
   }
   {
      LinearExtent temp = third.intersection(extent);

      if (temp.Length() > 0)
      {
	 temp.MoveLeft(third.Offset());
	 assert(temp.Offset() < 2);
	 assert((temp.Offset() + temp.Length()) <= 2);
	 vec.SetVec(start_index++, optend_ + temp.Offset(), temp.Length());
      }
   }
}

unsigned int
TelnetParser::OptionNegotiation::NumSubGroups(const LinearExtent &extent) const
{
   LinearExtent me(0, 3);
   return((extent.intersection(me).Length() > 0) ? 1 : 0);
}

void TelnetParser::OptionNegotiation::FillGroupVec(const LinearExtent &extent,
						   GroupVector &vec,
						   unsigned int &start_index)
{
   LinearExtent me(0, 3);
   me = me.intersection(extent);
   if (me.Length() > 0)
   {
      vec.SetVec(start_index++, buf_ + me.Offset(), me.Length());
   }
}
