#ifndef _UNEVT_UNIXConditionManager_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1998/04/29 01:59:58  hopper
// These are an attempt at a much more complex and unified architecture.  For
// now, they are not being used at all, but I may want to bring them back
// someday.
//

#include <bool.h>
#ifndef _UNEVT_OSConditionManager_H_
#  include <UniEvent/OSConditionManager.h>
#endif
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_UNIXConditionManager_H_

class UNIXObConditions;
class UNIXFDConditions;
class UNIXSigConditions;

class UNIXConditionManager : public OSConditionManager {
 public:
   static const UNEVT_ClassIdent identifier;

   UNIXConditionManager();
   virtual ~UNIXConditionManager();

   virtual bool HasConditions() const;
   virtual void WaitConditions(UNIDispatcher &post_to,
			       WaitType wait = Wait_FOREVER,
			       sec_t secs = 1, usec_t usecs = 0);

   inline bool AddCondition(UNIXObConditions *uobc);
   inline void RemoveCondition(UNIXObConditions *uobc);
   inline bool ContainsCondition(UNIXObConditions *uobc);

   UNIXObConditions *ConditionForObject(const UNIXObConditions &ob);

   // This class will only take conditions derived from UNIXObConditions.
   virtual OSObConds *CheckType(OSObConds *obc);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual bool i_AddCondition(OSObConds *obc) = 0;
   virtual void i_RemoveCondition(OSObConds *obc) = 0;
   virtual bool i_ContainsCondition(OSObConds *obc) const = 0;

   virtual void UpdateCondition(UNIXFDCondition *c);
   virtual void UpdateCondition(UNIXSigCondition *c);

 private:
   class Imp;

   Imp &imp_;

   UNIXConditionManager(const UNIXConditionManager &b);
   void operator =(const UNIXConditionManager &b);
};
   
#endif
