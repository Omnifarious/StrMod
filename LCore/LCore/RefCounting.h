#ifndef _EH_0_RefCounting_H_

/* $Header$ */

// $Log$
// Revision 1.1  1996/03/26 16:23:02  hopper
// Moved ReferenceCounting class over from Container library.
// Fixed up class numbering system so LCore uses a seperate library number
// from the container library.
//
// Revision 1.2  1996/02/22 03:56:03  hopper
// Fixed to be close to the new 'standard' class format.
// Fixed to be less open to subclasses.
//
// Revision 1.1.1.1  1996/02/20 02:32:52  hopper
// Imported into CVS
//
// Revision 0.2  1994/10/30  04:49:32  hopper
// Moved various things into the new LCore library.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#ifdef __GNUG__
#  pragma interface
#endif

#ifndef _LCORE_Protocol_H_
#  include <LCore/Protocol.h>
#endif

#define _EH_0_RefCounting_H_

class ReferenceCounting : virtual public Protocol {
 public:
   static const LCore_ClassIdent identifier;

   ReferenceCounting(U4Byte refs) : refcounter(refs)        {}
   virtual ~ReferenceCounting()                             {}

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || Protocol::AreYouA(cid));
   }

   void AddReference()                      { refcounter++; }
   void DelReference()                      { if (refcounter) refcounter--; }
   U4Byte NumReferences()                   { return(refcounter); }

 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

   void AddReferences(U4Byte num)           { refcounter += num; }
   inline void DelReferences(U4Byte num);

 private:
   U4Byte refcounter;
};

inline const ClassIdent *ReferenceCounting::i_GetIdent() const
{
   return(&identifier);
}

inline void ReferenceCounting::DelReferences(U4Byte num)
{
   if (refcounter > num)
      refcounter -= num;
   else
      refcounter = 0;
}

#endif