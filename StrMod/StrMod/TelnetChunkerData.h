#ifndef _STR_TelnetChunkerData_H_  // -*-c++-*-

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

#include <StrMod/TelnetChunker.h>
#include <StrMod/TelnetChars.h>
#include <StrMod/StrChunk.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/BufferChunk.h>
#include <LCore/GenTypes.h>
#include <cstddef>

#define _STR_TelnetChunkerData_H_

/** \class TelnetChunker::TelnetData TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * This is just a base abstract class for all things recognized as
 * telnet protocol messages.  It largely exists so the AreYouA() style
 * searches can be used to find out if a StrChunk is a telnet message
 * or not.
 */
class TelnetChunker::TelnetData : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   //! Constructors for abstract base classes don't do much.
   TelnetData()                                         { }
   //! Destructors for abstract base classes don't do much.
   virtual ~TelnetData()                                { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   // Redeclare this just to show we know what we're doing and it's still
   // abstract.
   virtual unsigned int Length() const = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   // Redeclare this just to show we know what we're doing and it's still
   // abstract.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation) = 0;
};

//---

/** \class TelnetChunker::SingleChar TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * A single character telnet command.
 */
class TelnetChunker::SingleChar : public TelnetChunker::TelnetData {
 public:
   static const STR_ClassIdent identifier;

   //! Construct a SingleChar for a particular command.
   inline SingleChar(TelnetChars::Commands opt);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                  { return(2); }

   //! What single character command am I?
   TelnetChars::Commands getCommand() const             { return(opt_); }

   //! Is this octet a valid single character telnet command?
   inline static bool isSpecial(U1Byte c);

   //! Convert the character to a member of the TelnetChars::Commands enum.
   inline static TelnetChars::Commands charToCommand(U1Byte c)
      throw(domain_error);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

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
   static const STR_ClassIdent identifier;

   //! Construct a telnet suboption request.
   inline Suboption(U1Byte type,
		    const StrChunkPtrT<BufferChunk> &cooked,
		    const StrChunkPtr &raw);
   /** Construct a telnet suboption request, generating the 'raw' data.
    *
    * The generated raw data is created by replacing any instances of IAC in
    * the cooked data with IAC IAC.
    */
   Suboption(U1Byte type, const StrChunkPtrT<BufferChunk> &cooked);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual unsigned int Length() const;

   //! Which suboption is this data for?
   inline U1Byte getType() const                       { return(optstart_[2]); }
   //! What's the suboption data with all the escapes processed?
   inline const StrChunkPtrT<BufferChunk> &getCooked() const;
   //! What's the suboption data without any escape processing?
   inline const StrChunkPtr &getRaw() const            { return(raw_); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   //! Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   static const U1Byte optend_[2];  // Always <IAC> <SE>  (255 240)
   U1Byte optstart_[3];  // Always <IAC> <SB> type
   const StrChunkPtr raw_;
   const unsigned int rawlen_;
   // The contents stripped of escape characters and such.
   const StrChunkPtrT<BufferChunk> cooked_;

   static const StrChunkPtr
   cookedToRaw(const StrChunkPtrT<BufferChunk> &cooked);
};

//---

/** \class TelnetChunker::OptionNegotiation TelnetChunkerData.h StrMod/TelnetChunkerData.h
 * A telnet suboption negotiation request.
 */
class TelnetChunker::OptionNegotiation : public TelnetChunker::TelnetData {
 public:
   static const STR_ClassIdent identifier;

   //! Create a particular kind of option negotiation for a particular option.
   inline OptionNegotiation(TelnetChars::OptionNegotiations request,
                            U1Byte type);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                  { return(3); }

   //! What kind of negotiation is being made?
   TelnetChars::OptionNegotiations getRequest() const   { return(request_); }
   //! What option is being negotiated?
   U1Byte getType() const                               { return(buf_[2]); }

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   TelnetChars::OptionNegotiations request_;
   U1Byte buf_[3];  //!< Always <IAC> <request> <type>
};

//-----------------------------inline functions--------------------------------

inline int TelnetChunker::TelnetData::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

//---

inline TelnetChunker::SingleChar::SingleChar(TelnetChars::Commands opt)
     : opt_(opt)
{
   buf_[0] = TelnetChars::IAC;
   buf_[1] = opt_;
}

inline int TelnetChunker::SingleChar::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || TelnetData::AreYouA(cid));
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
   U1Byte type, const StrChunkPtrT<BufferChunk> &cooked, const StrChunkPtr &raw)
     : raw_(raw), rawlen_(raw->Length()), cooked_(cooked)
{
   optstart_[0] = TelnetChars::IAC;
   optstart_[1] = TelnetChars::SB;
   optstart_[2] = type;
}

inline int TelnetChunker::Suboption::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || TelnetData::AreYouA(cid));
}

inline unsigned int TelnetChunker::Suboption::Length() const
{
   return(5 + rawlen_);
}

inline const StrChunkPtrT<BufferChunk> &
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

inline int
TelnetChunker::OptionNegotiation::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || TelnetData::AreYouA(cid));
}

#endif
