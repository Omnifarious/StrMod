#ifndef _UNEVT_StandCondManager_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1997/05/19 15:51:19  hopper
// Added new classes to implement some standard ConditionManger and
// ObjectConditions functionality.
//

#ifndef _UNEVT_OSConditionManager_H_
#  include <UniEvent/OSConditionManager.h>
#endif
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_StandCondManager_H_

class StandCondManager : public OSConditionManager {
 public:
   static const UNEVT_ClassIdent identifier;

   StandCondManager();
   virtual ~StandCondManager();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual bool HasConditions() const;
   virtual void WaitConditions(UNIDispatcher &post_to,
			       WaitType wait = Wait_FOREVER,
			       sec_t secs = 1, usec_t usecs = 0) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual bool i_AddCondition(OSObConds *obc) = 0;
   virtual void i_RemoveCondition(OSObConds *obc) = 0;
   virtual bool i_ContainsCondition(OSObConds *obc) const = 0;

 private:
   struct Imp;
   class compareObConds;

   Imp &imp_;

   StandCondManager(const StandCondManager &b);
   void operator =(const StandCondManager &b);
};

//-----------------------------inline functions--------------------------------

int StandCondManager::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || OSConditionManager::AreYouA(cid));
}

#endif
