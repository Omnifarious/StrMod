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

class OSConditionManager : virtual public Protocol {
 public:
   enum WaitType { Wait_POLL, Wait_FOREVER, Wait_TIME };
   typedef unsigned long sec_t;
   typedef unsigned int usec_t;
   static const UNEVT_ClassIdent identifier;

   OSConditionManager()                                { }
   virtual ~OSConditionManager()                       { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual void WaitConditions(WaitType wait = Wait_FOREVER,
			       sec_t secs = 1, usec_t usecs = 0) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

 private:
   OSConditionManager(const OSConditionManager &b);
   void operator =(const OSConditionManager &b);
};

//-----------------------------inline functions--------------------------------


inline int OSConditionManager::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
