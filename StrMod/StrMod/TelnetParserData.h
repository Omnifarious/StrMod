#ifndef _STR_TelnetParserData_H_  // -*-c++-*-

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

//: This is just a base abstract class for all things recognized as telnet
//: protocol messages.
class TelnetParser::TelnetData : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   TelnetData()                                         { }
   virtual ~TelnetData()                                { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const = 0;

   virtual unsigned int NumSubGroups() const = 0;

   virtual unsigned int NumSubGroups(const LinearExtent &extent) const = 0;

   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //: No child will need to implement this.
   virtual void i_DropUnused(const LinearExtent &usedextent,
			     KeepDir keepdir)           { }
};

//---

class TelnetParser::SingleChar : public TelnetParser::TelnetData {
 public:
   enum Specials { TEOF = 236, SUSP = 237, ABORT = 238, EOR = 239, NOP = 241,
		   DM = 242, BRK = 243, IP = 244, AO = 245, AYT = 246,
		   EC = 247, EL = 248, GA = 249 };

   static const STR_ClassIdent identifier;

   inline SingleChar(Specials opt);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                  { return(2); }

   virtual unsigned int NumSubGroups() const            { return(1); }

   virtual unsigned int NumSubGroups(const LinearExtent &extent) const;

   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   Specials getType() const                             { return(opt_); }

   inline static bool isSpecial(U1Byte c);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

 private:
   const Specials opt_;
   U1Byte buf_[2];  // Always <IAC> opt_
};

//---

class TelnetParser::Suboption : public TelnetParser::TelnetData {
 public:
   static const STR_ClassIdent identifier;

   //: Construct a telnet suboption request.
   inline Suboption(U1Byte type,
		    const StrChunkPtrT<BufferChunk> &cooked,
		    const StrChunkPtr &raw);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual unsigned int Length() const;

   inline virtual unsigned int NumSubGroups() const;

   virtual unsigned int NumSubGroups(const LinearExtent &extent) const;

   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   inline U1Byte getType() const                       { return(optstart_[2]); }

   inline const StrChunkPtrT<BufferChunk> &getCooked() const;
   inline const StrChunkPtr &getRaw() const            { return(raw_); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   static const U1Byte optend_[2];  // Always <IAC> <SE>  (255 240)
   U1Byte optstart_[3];  // Always <IAC> <SB> type
   const StrChunkPtr raw_;
   const unsigned int rawlen_;
   // The contents stripped of escape characters and such.
   const StrChunkPtrT<BufferChunk> cooked_;
};

//---

class TelnetParser::OptionNegotiation : public TelnetParser::TelnetData {
 public:
   enum Requests { WILL = 251, WONT = 252, DO = 253, DONT = 254 };

   static const STR_ClassIdent identifier;

   inline OptionNegotiation(Requests request, U1Byte type);

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                  { return(3); }

   virtual unsigned int NumSubGroups() const            { return(1); }

   virtual unsigned int NumSubGroups(const LinearExtent &extent) const;

   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   Requests getRequest() const                          { return(request_); }

   U1Byte getType() const                               { return(buf_[2]); }

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

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

inline unsigned int TelnetParser::Suboption::NumSubGroups() const
{
   assert(raw_->NumSubGroups() == 1);
   // I know that BufferChunk s only have 1 subgroup.
   return(3);
}

inline const StrChunkPtrT<BufferChunk> &
TelnetParser::Suboption::getCooked() const
{
   return(cooked_);
}

//---

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
