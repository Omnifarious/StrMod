#ifndef _STRT_TelnetToAscii_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../Changelog

#include <StrMod/StreamProcessor.h>
#include <StrMod/StrChunkPtr.h>
#include <LCore/HopClTypes.h>
#include <string>

#define _STRT_TelnetToAscii_H_

class TelnetToAscii : public StreamProcessor {
 public:
   static const EH_ClassIdent identifier;

   explicit TelnetToAscii(bool stripdata = false);
   explicit TelnetToAscii(const string &name, bool stripdata = false);
   virtual ~TelnetToAscii()                                          { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual void processIncoming();

 private:
   StrChunkPtr namechunk_;
   bool stripdata_;
};

//-----------------------------inline functions--------------------------------

int TelnetToAscii::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

#endif
