#ifndef _STR_TelnetParser_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/StreamProcessor.h>
#include <StrMod/STR_ClassIdent.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/PreAllocBuffer.h>
#include <StrMod/DynamicBuffer.h>
#include <LCore/GenTypes.h>
#include <cstddef>

#define _STR_TelnetParser_H_

class TelnetParser : public StreamProcessor {
 public:
   class TelnetData;
   class SingleChar;
   class Suboption;
   class OptionNegotiation;

   static const STR_ClassIdent identifier;
   static const U1Byte TN_IAC = 255U;
   static const U1Byte TN_SB = 250U;
   static const U1Byte TN_SE = 240U;

   TelnetParser();
   virtual ~TelnetParser();

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   enum Actions { PC_AddTelnetBlock, PC_AddCurrentChar, PC_SkipAddCurrent,
		  PC_AddPrevIACAndCurrentChar, PC_DoNothing };

   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void processIncoming();

   Actions processChar(U1Byte ch);

 private:
   enum ParseStates { PS_Normal, PS_Escape, PS_SubNeg,
		      PS_SuboptNum, PS_Subopt, PS_SuboptEscape };
   typedef PreAllocBuffer<32> SuboptBuffer;

   static const size_t subopt_size_limit_ = 65536U;

   size_t incoming_pos_;
   StrChunkPtr telnetcommand_;

   // Parser stuff.
   ParseStates state_;
   // For collecting data.
   StrChunkPtrT<DynamicBuffer> data_;
   size_t datasize_;
   // For collecting suboption data
   StrChunkPtrT<SuboptBuffer> cookedsubopt_;
   size_t cookedsize_;
   StrChunkPtrT<SuboptBuffer> rawsubopt_;
   size_t rawsize_;
   U1Byte negtype_;    // Used to indicate WILL, WONT, DO, or DONT
   U1Byte suboptnum_;  // Used when a suboption # is part of the state.

   inline static void addToChunk(StrChunkPtrT<SuboptBuffer> &chnk,
				 size_t &cursize, U1Byte ch);
};

//-----------------------------inline functions--------------------------------

inline int TelnetParser::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

#endif
