#ifndef _LCORE_Protocol_H_

/* $Header$ */

// $Log$
// Revision 1.2  1996/03/26 16:23:01  hopper
// Moved ReferenceCounting class over from Container library.
// Fixed up class numbering system so LCore uses a seperate library number
// from the container library.
//
// Revision 1.1.1.1  1995/07/22 04:09:25  hopper
// Imported sources
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
   static const LCore_ClassIdent identifier;

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
