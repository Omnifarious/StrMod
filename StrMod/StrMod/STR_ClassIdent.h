#ifndef _STR_STR_ClassIdent_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.3  1999/03/05 15:30:32  hopper
// Some lint cleanup changes, and also cleanups and fixes to the code
// that handles interplug communication and notification.
//
// Revision 1.2  1996/02/12 05:50:04  hopper
// Added operator == to work around g++ 2.7.2 bug.
//
// Revision 1.1.1.1  1995/07/22 04:46:51  hopper
// Imported sources
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

//     inline bool operator ==(const ClassIdent &b) const;

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

//  inline bool STR_ClassIdent::operator ==(const ClassIdent &b) const
//  {
//     return(EH_ClassIdent::operator ==(b));
//  }

inline STR_ClassIdent::STR_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::StrMod, cnum))
{
}

#endif
