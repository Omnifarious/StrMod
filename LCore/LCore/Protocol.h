#ifndef _LCORE_Protocol_H_

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:09:25  hopper
// Initial revision
//
// Revision 0.2  1994/10/30  04:41:41  hopper
// Moved various things into the new LCore library.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#ifdef __GNUG__
#  pragma interface
#endif

#ifndef _LCORE_HopClTypes_H_
#  include <LCore/HopClTypes.h>
#endif

#define _LCORE_Protocol_H_

class Protocol {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const EH0_ClassIdent identifier;

   const ClassIdent &GetIdent() const               { return(*i_GetIdent()); }
   virtual int AreYouA(const ClassIdent &cid) const {
      return(identifier == cid);
   }

   Protocol()                                       {}
   virtual ~Protocol()                              {}
};

inline const ClassIdent *Protocol::i_GetIdent() const
{
   return(&identifier);
}

#endif
