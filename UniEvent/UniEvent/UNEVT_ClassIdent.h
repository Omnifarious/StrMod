#ifndef _UNEVT_UNEVT_ClassIdent_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/HopClTypes.h>

#define _UNEVT_UNEVT_ClassIdent_H_

class UNEVT_ClassIdent : public EH_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const UNEVT_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline UNEVT_ClassIdent(U4Byte cnum);
};

//---------------------------inline functions--------------------------------

inline const ClassIdent *UNEVT_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int UNEVT_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

inline UNEVT_ClassIdent::UNEVT_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::User3, cnum))
{
}

#endif
