#ifndef _UNEVT_OSObjectConditions_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1997/05/18 23:33:10  hopper
// Firmed up interface for OSConditionManager, and OSObjectConditions
// classes, and related member functions in other classes.
//

#include <LCore/Protocol.h>
#include <bool.h>
#include <UniEvent/UNEVT_ClassIdent.h>

#define _UNEVT_OSObjectConditions_H_

class UNIEventPtr;
class OSConditionManager;

class OSObjectConditions : virtual public Protocol {
 public:
   static const UNEVT_ClassIdent identifier;

   OSObjectConditions()                                { }
   virtual ~OSObjectConditions()                       { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual const UNIEventPtr OnTrueTrigger(const UNIEventPtr &event) = 0;

   inline bool RegisterWith(OSConditionManager *cm);
   inline void UnregisterFrom(OSConditionManager *cm);
   inline bool RegisteredWith(OSConditionManager *cm) const;

   inline virtual OSConditionManager *CheckType(OSConditionManager *cm) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual bool i_RegisterWith(OSConditionManager *cm) = 0;
   virtual void i_UnregisterFrom(OSConditionManager *cm) = 0;
   virtual bool i_RegisteredWith(OSConditionManager *cm) const = 0;

 private:
   OSObjectConditions(const OSObjectConditions &b);
   void operator =(const OSObjectConditions &b);
};

//-----------------------------inline functions--------------------------------

inline int OSObjectConditions::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline bool OSObjectConditions::RegisterWith(OSConditionManager *cm)
{
   OSConditionManager *tccm = CheckType(cm);

   if (tccm) {
      return(i_RegisterWith(tccm));
   } else {
      return(false);
   }
}

inline void OSObjectConditions::UnregisterFrom(OSConditionManager *cm)
{
   OSConditionManager *tccm = CheckType(cm);

   if (tccm) {
      i_UnregisterFrom(tccm);
   }
}

inline bool OSObjectConditions::RegisteredWith(OSConditionManager *cm) const
{
   OSConditionManager *tccm = CheckType(cm);

   if (tccm) {
      return(i_RegisteredWith(tccm));
   } else {
      return(false);
   }
}

inline
OSConditionManager *OSObjectConditions::CheckType(OSConditionManager *cm) const
{
   return(cm);
}

#endif
