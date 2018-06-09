#ifndef _STR_TelnetChunkerData_H_  // -*-c++-*-

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

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <LCore/GenTypes.h>
#include <StrMod/TelnetChunker.h>
#include <StrMod/TelnetChars.h>
#include <StrMod/StrChunk.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/BufferChunk.h>

#define _STR_TelnetChunkerData_H_

namespace strmod {
namespace strmod {

/** \class TelnetChunker::TelnetData TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * This is just a base abstract class for all things recognized as
 * telnet protocol messages.  It largely exists so the AreYouA() style
 * searches can be used to find out if a StrChunk is a telnet message
 * or not.
 */
class TelnetChunker::TelnetData : public StrChunk
{
 public:
   //! Constructors for abstract base classes don't do much.
   TelnetData() = default;
   //! Destructors for abstract base classes don't do much.
   virtual ~TelnetData() = default;

   // Redeclare this just to show we know what we're doing and it's still
   // abstract.
   unsigned int Length() const override = 0;

 protected:
   typedef lcore::U1Byte U1Byte;

   // Redeclare this just to show we know what we're doing and it's still
   // abstract.
   void acceptVisitor(ChunkVisitor &visitor) override = 0;
};

//---

/** \class TelnetChunker::SingleChar TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * A single character telnet command.
 */
class TelnetChunker::SingleChar : public TelnetChunker::TelnetData
{
 public:
   //! Construct a SingleChar for a particular command.
   inline SingleChar(TelnetChars::Commands opt);

   unsigned int Length() const override                 { return(2); }

   //! What single character command am I?
   TelnetChars::Commands getCommand() const             { return(opt_); }

   //! Is this octet a valid single character telnet command?
   inline static bool isSpecial(U1Byte c);

   //! Convert the character to a member of the TelnetChars::Commands enum.
   inline static TelnetChars::Commands charToCommand(U1Byte c);

 protected:
   void acceptVisitor(ChunkVisitor &visitor) override;

 private:
   const TelnetChars::Commands opt_;
   U1Byte buf_[2];  // Always <IAC> opt_
};

//---

/** \class TelnetChunker::Suboption TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * A telnet suboption.
 * This should never be seen for a particular option unless we've previously
 * agreed to talk about this option in an OptionNegotiation.
 */
class TelnetChunker::Suboption : public TelnetChunker::TelnetData {
 public:
   typedef ::std::shared_ptr<BufferChunk> bufchnkptr_t;
   //! Construct a telnet suboption request.
   inline Suboption(U1Byte type,
		    const bufchnkptr_t &cooked,
		    const StrChunkPtr &raw);
   /** Construct a telnet suboption request, generating the 'raw' data.
    *
    * The generated raw data is created by replacing any instances of IAC in
    * the cooked data with IAC IAC.
    */
   Suboption(U1Byte type, const bufchnkptr_t &cooked);

   inline unsigned int Length() const override;

   //! Which suboption is this data for?
   inline U1Byte getType() const                       { return(optstart_[2]); }
   //! What's the suboption data with all the escapes processed?
   inline const bufchnkptr_t &getCooked() const;
   //! What's the suboption data without any escape processing?
   inline const StrChunkPtr &getRaw() const            { return(raw_); }

 protected:
   //! Accept a ChunkVisitor, and maybe lead it through your children.
   void acceptVisitor(ChunkVisitor &visitor) override;

 private:
   static const U1Byte optend_[2];  // Always <IAC> <SE>  (255 240)
   U1Byte optstart_[3];  // Always <IAC> <SB> type
   const StrChunkPtr raw_;
   const unsigned int rawlen_;
   // The contents stripped of escape characters and such.
   const bufchnkptr_t cooked_;

   static const StrChunkPtr
   cookedToRaw(const bufchnkptr_t &cooked);
};

//---

/** \class TelnetChunker::OptionNegotiation TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * A telnet suboption negotiation request.
 */
class TelnetChunker::OptionNegotiation : public TelnetChunker::TelnetData {
 public:
   //! Create a particular kind of option negotiation for a particular option.
   inline OptionNegotiation(TelnetChars::OptionNegotiations request,
                            U1Byte type);

   unsigned int Length() const override                 { return(3); }

   //! What kind of negotiation is being made?
   TelnetChars::OptionNegotiations getRequest() const   { return(request_); }
   //! What option is being negotiated?
   U1Byte getType() const                               { return(buf_[2]); }

 protected:
   void acceptVisitor(ChunkVisitor &visitor) override;

 private:
   TelnetChars::OptionNegotiations request_;
   U1Byte buf_[3];  //!< Always <IAC> <request> <type>
};

//-----------------------------inline functions--------------------------------

inline TelnetChunker::SingleChar::SingleChar(TelnetChars::Commands opt)
     : opt_(opt)
{
   buf_[0] = TelnetChars::IAC;
   buf_[1] = opt_;
}

inline bool TelnetChunker::SingleChar::isSpecial(U1Byte c)
{
   return(((c >= TelnetChars::TEOF) && (c <= TelnetChars::EOR)) ||
          ((c >= TelnetChars::NOP) && (c <= TelnetChars::GA)));
}

//---

/**
 * \param type Which suboption is this for?
 * \param cooked The suboption data that has had the escape characters
 *               processed.
 * \param raw The raw suboption data with no escape processing at all.
 */
inline
TelnetChunker::Suboption::Suboption(
   U1Byte type, const bufchnkptr_t &cooked, const StrChunkPtr &raw)
     : raw_(raw), rawlen_(raw->Length()), cooked_(cooked)
{
   optstart_[0] = TelnetChars::IAC;
   optstart_[1] = TelnetChars::SB;
   optstart_[2] = type;
}

inline unsigned int TelnetChunker::Suboption::Length() const
{
   return(5 + rawlen_);
}

inline const ::std::shared_ptr<BufferChunk> &
TelnetChunker::Suboption::getCooked() const
{
   return(cooked_);
}

//---

/**
 * \param request The kind of negotiation
 * \param type The option is being negotiated
 */
inline
TelnetChunker::OptionNegotiation::OptionNegotiation(
   TelnetChars::OptionNegotiations request,
   U1Byte type)
     : request_(request)
{
   assert((request >= TelnetChars::O_WILL) && (request <= TelnetChars::O_DONT));
   buf_[0] = 255;
   buf_[1] = request_;
   buf_[2] = type;
}

}  // namespace strmod
}  // namespace strmod

#endif
