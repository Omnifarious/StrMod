#ifndef _UNEVT_OSConditionManager_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// Log can be found in ChangeLog

#include <LCore/Protocol.h>
#include <bool.h>
#include <UniEvent/UNEVT_ClassIdent.h>

#define _UNEVT_OSConditionManager_H_

class UNIDispatcher;
class OSObjectConditions;

class OSConditionManager : virtual public Protocol {
 public:
   enum WaitType { Wait_POLL, Wait_FOREVER, Wait_TIME };
   typedef OSObjectConditions OSObConds;
   typedef unsigned long sec_t;
   typedef unsigned int usec_t;
   static const UNEVT_ClassIdent identifier;

   OSConditionManager()                                { }
   virtual ~OSConditionManager()                       { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual bool HasConditions() const = 0;
   virtual void WaitConditions(WaitType wait = Wait_FOREVER,
			       sec_t secs = 1, usec_t usecs = 0) = 0;

   inline bool AddCondition(OSObConds *obc);
   inline void RemoveCondition(OSObConds *obc);
   inline bool ContainsCondition(OSObConds *obc) const;

   inline virtual OSObConds *CheckType(OSObConds *obc) const;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual bool i_AddCondition(OSObConds *obc) = 0;
   virtual void i_RemoveCondition(OSObConds *obc) = 0;
   virtual bool i_ContainsCondition(OSObConds *obc) const = 0;

 private:
   OSConditionManager(const OSConditionManager &b);
   void operator =(const OSConditionManager &b);
};

//-----------------------------inline functions--------------------------------


inline int OSConditionManager::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline bool OSConditionManager::AddCondition(OSObConds *obc)
{
   OSObConds *tcobc = CheckType(obc);

   if (tcobc) {
      return(i_AddCondition(tcobc));
   } else {
      return(false);
   }
}

inline void OSConditionManager::RemoveCondition(OSObConds *obc)
{
   OSObConds *tcobc = CheckType(obc);

   if (tcobc) {
      i_RemoveCondition(tcobc);
   }
}

inline bool OSConditionManager::ContainsCondition(OSObConds *obc) const
{
   OSObConds *tcobc = CheckType(obc);

   if (tcobc) {
      return(i_ContainsCondition(tcobc));
   } else {
      return(false);
   }
}

inline OSObjectConditions *OSConditionManager::CheckType(OSObConds *obc) const
{
   return(obc);
}

#endif
