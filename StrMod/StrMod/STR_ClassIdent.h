#ifndef _STR_STR_ClassIdent_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
// Revision 1.2  1995/04/05  04:52:07  hopper
// Fixed a stupid mistake.
//
// Revision 1.1  1995/03/08  05:34:44  hopper
// Initial revision
//

#include <LCore/HopClTypes.h>

#define _STR_STR_ClassIdent_H_

class STR_ClassIdent : public EH_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline STR_ClassIdent(U4Byte cnum);
};

//---------------------------inline functions--------------------------------

inline const ClassIdent *STR_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int STR_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

inline STR_ClassIdent::STR_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::StrMod, cnum))
{
}

#endif
