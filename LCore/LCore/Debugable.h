#ifndef _LCORE_Debugable_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <LCore/HopClTypes.h>
#include <LCore/Protocol.h>

#define _LCORE_Debugable_H_

class ostream;

class Debugable : virtual public Protocol {
 public:
   static const LCore_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: This should ALWAYS return true.  It says whether the class invariant
   //: holds or not.
   // <p>Your class invariant should be as strict as possible.  The class
   // invariant is used to detect when the class goes into a bad state, so all
   // possible bad states should be caught by the invariant.</p>
   virtual bool invariant() const = 0;

   //: This should print out as much of the internal state of a class as would
   //: be needed to debug it properly.
   virtual void printState(ostream &os) const = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }
};

//-----------------------------inline functions--------------------------------

inline int Debugable::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
