#ifndef _STR_PassThrough_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../Changelog
//
// Revision 1.1  1996/09/02 23:26:48  hopper
// Added PassThrough class to use when you needed a StreamProcessor that
// did nothing except buffer one Chunk worth of data.
//

#include <StrMod/StreamProcessor.h>
#include <cassert>

#define _STR_PassThrough_H_

class PassThrough : public StreamProcessor {
 public:
   static const STR_ClassIdent identifier;

   PassThrough()                                       { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual void processIncoming();

 private:
   PassThrough(const PassThrough &b);
};

//-----------------------------inline functions--------------------------------

inline int PassThrough::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

inline void PassThrough::processIncoming()
{
   assert(!outgoing_);
   outgoing_ = incoming_;
   outgoing_ready_ = true;
   incoming_.ReleasePtr();
}

#endif
