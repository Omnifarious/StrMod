#ifndef _NET_NET_ClassIdent_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
//
// Revision 1.1  1996/02/12 03:01:54  hopper
// Added links to my ClassIdent system.
//

#include <LCore/HopClTypes.h>

#define _NET_NET_ClassIdent_H_

class NET_ClassIdent : public EH_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const NET_ClassIdent identifier;

   inline NET_ClassIdent(U4Byte cnum);

   inline virtual int AreYouA(const ClassIdent &cid) const;
};

//---------------------------inline functions--------------------------------

inline NET_ClassIdent::NET_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::Net, cnum))
{
}

inline const ClassIdent *NET_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int NET_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

#endif
