#ifndef _STR_TelnetParserData_H_  // -*-c++-*-

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

#include <StrMod/TelnetParser.h>
#include <StrMod/StrChunk.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/StrChunkPtrT.h>
#include <LCore/GenTypes.h>
#include <cstddef>

#define _STR_TelnetParserData_H_

/** \class TelnetParser::TelnetData TelnetParserData.h StrMod/TelnetParserData.h
 * This is just a base abstract class for all things recognized as
 * telnet protocol messages.  It largely exists so the AreYouA() style
 * searches can be used to find out if a StrChunk is a telnet message
 * or not.
 */
class TelnetParser::TelnetData : public StrChunk {
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

/** \class TelnetParser::SingleChar TelnetParserData.h StrMod/TelnetParserData.h
 * A single character telnet command.
 */
class TelnetParser::SingleChar : public TelnetParser::TelnetData {
 public:
   /** The names and octet values of the various single character telnet commands.
    * These are defined in RFCs <a
    * href="http://info.internet.isi.edu/in-notes/rfc/files/rfc854.txt">854</a>,
    * <a
    * href="http://info.internet.isi.edu/in-notes/rfc/files/rfc885.txt">885</a>,
    * and <a
    * href="http://info.internet.isi.edu/in-notes/rfc/files/rfc1184.txt">1184</a>
    */
   enum Specials {
      TEOF = 236, //!< End Of File, called TEOF to avoid conflicting with EOF macro
      SUSP = 237, //!< Suspend process
      ABORT = 238, //!< Abort process
      EOR = 239, //!< End of record
      NOP = 241, //!< No operation
      DM = 242, //!< Data mark, use with Synch option, requires out-of-band data support
      BRK = 243, //!< Break  (often a long string of 0 bits in RS232)
      IP = 244, //!< Interrupt process
      AO = 245, //!< Abort output
      AYT = 246, //!< Are you there?  Remote side expected to reply with 'Yes'
      EC = 247, //!< Erase character (like backspace)
      EL = 248, //!< Erase line
      GA = 249  //!< Go ahead (for half duplex (the default) connections)
   };

   static const STR_ClassIdent identifier;

   //! Construct a SingleChar for a particular command.
   inline SingleChar(Specials opt);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                  { return(2); }

   //! What kind of single character command am I?
   Specials getType() const                             { return(opt_); }

   //! Is this octet a valid single character telnet command?
   inline static bool isSpecial(U1Byte c);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //! Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   const Specials opt_;
   U1Byte buf_[2];  // Always <IAC> opt_
};

//---

/** \class TelnetParser::Suboption TelnetParserData.h StrMod/TelnetParserData.h
 * A telnet suboption.
 * This should never be seen for a particular option unless we've previously
 * agreed to talk about this option in an OptionNegotiation.
 */
class TelnetParser::Suboption : public TelnetParser::TelnetData {
 public:
   static const STR_ClassIdent identifier;

   //! Construct a telnet suboption request.
   inline Suboption(U1Byte type,
		    const StrChunkPtrT<BufferChunk> &cooked,
		    const StrChunkPtr &raw);

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
};

//---

/** \class TelnetParser::OptionNegotiation TelnetParserData.h StrMod/TelnetParserData.h
 * A telnet suboption negotiation request.
 */
class TelnetParser::OptionNegotiation : public TelnetParser::TelnetData {
 public:
   /** The kind of negotiation being made.
    * The telnet protocol is designed as a peer-to-peer protocol, and so
    * either side can initiate an option negotiation to enable or disable an
    * option for either half (or direction) of the conversation.
    */
   enum Requests { WILL = 251,  //!< Sender wants to enable option for itself
                   WONT = 252,  //!< Sender wants to disable option for itself
                   DO = 253,   //!< Sender wants receiver to enable option
                   DONT = 254  //!< Sender wants receiver to disable option
   };

   static const STR_ClassIdent identifier;

   //! Create a particular kind of option negotiation for a particular option.
   inline OptionNegotiation(Requests request, U1Byte type);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                  { return(3); }

   //! What kind of negotiation is being made?
   Requests getRequest() const                          { return(request_); }
   //! What option is being negotiated?
   U1Byte getType() const                               { return(buf_[2]); }

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //! Accept a ChunkVisitor, and maybe lead it through your children.
   virtual void acceptVisitor(ChunkVisitor &visitor)
      throw(ChunkVisitor::halt_visitation);

 private:
   Requests request_;
   U1Byte buf_[3];  // Always <IAC> request type
};

//-----------------------------inline functions--------------------------------

inline int TelnetParser::TelnetData::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

//---

inline TelnetParser::SingleChar::SingleChar(Specials opt)
     : opt_(opt)
{
   assert(((opt >= TEOF) && (opt <= EOR)) || ((opt >= NOP) && (opt <= GA)));
   buf_[0] = 255;
   buf_[1] = opt_;
}

inline int TelnetParser::SingleChar::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || TelnetData::AreYouA(cid));
}

inline bool TelnetParser::SingleChar::isSpecial(U1Byte c)
{
   return(((c >= TEOF) && (c <= EOR)) || ((c >= NOP) && (c <= GA)));
}

//---

/**
 * \param type Which suboption is this for?
 * \param cooked The suboption data that has had the escape characters
 *               processed.
 * \param raw The raw suboption data with no escape processing at all.
 */
inline
TelnetParser::Suboption::
Suboption(U1Byte type,
	  const StrChunkPtrT<BufferChunk> &cooked,
	  const StrChunkPtr &raw)
     : raw_(raw), rawlen_(raw->Length()), cooked_(cooked)
{
   optstart_[0] = 255;
   optstart_[1] = 250;
   optstart_[2] = type;
}

inline int TelnetParser::Suboption::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || TelnetData::AreYouA(cid));
}

inline unsigned int TelnetParser::Suboption::Length() const
{
   return(5 + rawlen_);
}

inline const StrChunkPtrT<BufferChunk> &
TelnetParser::Suboption::getCooked() const
{
   return(cooked_);
}

//---

/**
 * \param request The kind of negotiation
 * \param type The option is being negotiated
 */
inline TelnetParser::OptionNegotiation::OptionNegotiation(Requests request,
							  U1Byte type)
     : request_(request)
{
   assert((request >= WILL) && (request <= DONT));
   buf_[0] = 255;
   buf_[1] = request_;
   buf_[2] = type;
}

inline int TelnetParser::OptionNegotiation::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || TelnetData::AreYouA(cid));
}

#endif
