#ifndef _STR_PassThrough_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/09/02 23:26:48  hopper
// Added PassThrough class to use when you needed a StreamProcessor that
// did nothing except buffer one Chunk worth of data.
//

#include <StrMod/StreamProcessor.h>

#define _STR_PassThrough_H_

class PassThrough : public StreamProcessor {
 public:
   typedef StreamProcessor parentclass;
   static const STR_ClassIdent identifier;

   PassThrough()                                       { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual void ProcessIncoming();

 private:
   PassThrough(const PassThrough &b);
};

//-----------------------------inline functions--------------------------------

inline int PassThrough::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || parentclass::AreYouA(cid));
}

inline void PassThrough::ProcessIncoming()
{
   m_outgoing = m_incoming;
   m_outgoing_ready = true;
   m_incoming.ReleasePtr();
}

#endif
