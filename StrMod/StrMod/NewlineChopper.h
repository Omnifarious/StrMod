#ifndef _STR_NewlineChopper_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
//
// Revision 1.1  1996/09/02 23:18:48  hopper
// Added NewlineChopper class so users would have a simple class that
// would break up and recombine streams using '\n' as a separator.
//

#include <StrMod/CharChopper.h>

#define _STR_NewlineChopper_H_

class NewlineChopper : public CharChopper {
 public:
   typedef CharChopper parentclass;
   static const STR_ClassIdent identifier;

   NewlineChopper() : CharChopper('\n')                { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   NewlineChopper(const NewlineChopper &b);
};

//-----------------------------inline functions--------------------------------

inline int NewlineChopper::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || parentclass::AreYouA(cid));
}

#endif
