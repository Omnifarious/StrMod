/* $Header$ */

// $Log$
// Revision 1.1  1997/05/19 15:51:05  hopper
// Added new classes to implement some standard ConditionManger and
// ObjectConditions functionality.
//

#ifdef __GNUG__
#  pragma implementation "StandObConditions.h"
#endif

#include "UniEvent/StandObConditions.h"
#include "UniEvent/OSConditionManager.h"
#include <set>
#include <string.h>   // To get memcmp

class StandObConditions::compareCondMans {
 public:
   typedef OSConditionManager *CompType;

   inline bool operator()(const CompType &a, const CompType &b) const;
};

inline bool
StandObConditions::compareCondMans::operator()(const CompType &a,
					       const CompType &b) const
{
   return(memcmp(&a, &b, sizeof(CompType)));
}

typedef
set<OSConditionManager *, StandObConditions::compareCondMans> CondManSet;

struct StandObConditions::Imp : public CondManSet {
   bool going_away_;
};

StandObConditions::StandObConditions() : imp_(*(new Imp))
{
   imp_.going_away_ = false;
}

StandObConditions::~StandObConditions()
{
   imp_.going_away_ = true;

   {
      Imp::iterator i, end;

      for (i = imp_.begin(), end = imp_.end(); i != end; ++i) {
	 (*i)->RemoveCondition(this);
      }
   }
   delete &imp_;
}

bool StandObConditions::i_RegisterWith(OSConditionManager *cm)
{
   if (imp_.going_away_) {
      return(false);
   } else {
      Imp::pair_iterator_bool result = imp_.insert(cm);

      if (result.second) {  // If something was actually inserted
	 if (!cm->AddCondition(this)) {
	    imp_.erase(result.first);
	    return(false);
	 }
      }
      return(true);
   }
}

void StandObConditions::i_UnregisterFrom(OSConditionManager *cm)
{
   if (!imp_.going_away_) {
      imp_.erase(cm);
      cm->RemoveCondition(this);
   }
}

bool StandObConditions::i_RegisteredWith(OSConditionManager *cm) const
{
   if (imp_.going_away_) {
      return(false);
   } else {
      return(imp_.find(cm) != imp_.end());
   }
}
