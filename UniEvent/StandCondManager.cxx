/* $Header$ */

// $Log$
// Revision 1.1  1997/05/19 15:51:04  hopper
// Added new classes to implement some standard ConditionManger and
// ObjectConditions functionality.
//

#ifdef __GNUG__
#  pragma implementation "StandCondManager.h"
#endif

#include "UniEvent/StandCondManager.h"
#include "UniEvent/OSObjectConditions.h"
#include <set>
#include <string.h>   // To get memcmp

class StandCondManager::compareObConds {
 public:
   typedef OSObConds *CompType;

   inline bool operator()(const CompType &a, const CompType &b) const;
};

inline bool
StandCondManager::compareObConds::operator()(const CompType &a,
					     const CompType &b) const
{
   return(memcmp(&a, &b, sizeof(CompType)));
}

typedef set<OSObjectConditions *, StandCondManager::compareObConds> ObCondSet;

struct StandCondManager::Imp : public ObCondSet {
   bool going_away_;
};

StandCondManager::StandCondManager() : imp_(*(new Imp))
{
   imp_.going_away_ = false;
}

StandCondManager::~StandCondManager()
{
   imp_.going_away_ = true;

   {
      Imp::iterator i, end;

      for (i = imp_.begin(), end = imp_.end(); i != end; ++i) {
	 (*i)->UnregisterFrom(this);
      }
   }
   delete &imp_;
}

bool StandCondManager::HasConditions() const
{
   return(imp_.size() != 0);
}

bool StandCondManager::i_AddCondition(OSObConds *obc)
{
   if (imp_.going_away_) {
      return(false);
   } else {
      Imp::pair_iterator_bool result = imp_.insert(obc);

      if (result.second) {  // If we actually inserted it.
	 if (!obc->RegisterWith(this)) {
	    imp_.erase(result.first);
	    return(false);
	 }
      }
      return(true);
   }
}

void StandCondManager::i_RemoveCondition(OSObConds *obc)
{
   if (!imp_.going_away_) {
      imp_.erase(obc);
      obc->UnregisterFrom(this);
   }
}

bool StandCondManager::i_ContainsCondition(OSObConds *obc) const
{
   if (imp_.going_away_) {
      return(false);
   } else {
      return(imp_.find(obc) != imp_.end());
   }
}
