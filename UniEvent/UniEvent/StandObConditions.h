#ifndef _UNEVT_StandObConditions_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1997/05/19 15:51:19  hopper
// Added new classes to implement some standard ConditionManger and
// ObjectConditions functionality.
//

#ifndef _UNEVT_OSObjectConditions_H_
#  include <UniEvent/OSObjectConditions.h>
#endif
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_StandObConditions_H_

class StandObConditions : public OSObjectConditions {
 public:
   static const UNEVT_ClassIdent identifier;

   StandObConditions();
   virtual ~StandObConditions();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual const UNIEventPtr OnTrueTrigger(const UNIEventPtr &event) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual bool i_RegisterWith(OSConditionManager *cm) = 0;
   virtual void i_UnregisterFrom(OSConditionManager *cm) = 0;
   virtual bool i_RegisteredWith(OSConditionManager *cm) const = 0;

 private:
   struct Imp;
   class compareCondMans;

   Imp &imp_;

   StandObConditions(const StandObConditions &b);
   void operator =(const StandObConditions &b);
};

//-----------------------------inline functions--------------------------------

inline int StandObConditions::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || OSObjectConditions::AreYouA(cid));
}

#endif
