/* $Header$ */

// $Log$
// Revision 1.1  1998/04/29 01:59:52  hopper
// These are an attempt at a much more complex and unified architecture.  For
// now, they are not being used at all, but I may want to bring them back
// someday.
//

#ifdef __GNUG__
#  pragma implementation "UNIXConditionManager.h"
#endif

#include "UniEvent/UNIXConditionManager.h"
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#if defined(MAXSIG) && !defined(NSIG)
#  define NSIG (MAXSIG + 1)
#endif

class UNIXFDConditions;
class UNIXSigConditions;
typedef UNIXFDConditions *UFDCondP;

struct UNIXConditionManager::Imp {
 public:
   UFDCondP *fdhandlers_;
   int max_fds_;
   UNIXSigConditions *sighandlers_[NSIG];
};

UNIXConditionManager::UNIXConditionManager() : imp_(*(new Imp))
{
   {
      int max_fds = sysconf(_SC_OPEN_MAX);

      assert(max_fds >= 0);
      imp_.max_fds_ = max_fds;
   }
   imp_.fdhandlers_ = new UFDCondP[imp_.max_fds_];

   {
      for (int i = 0; i < imp_.max_fds_; ++i) {
	 imp_.fdhandlers_[i] = 0;
      }
   }
   {
      for (int i = 0; i < NSIG; ++i) {
	 imp_.sighandlers_[i] = 0;
      }
   }
}

UNIXConditionManager::~UNIXConditionManager()
{
   {
      for (int i = 0; i < NSIG; ++i) {
	 if (imp_.sighandlers_[i]) {
	    i_RemoveCondition(imp_.sighandlers_[i]);
	 }
      }
   }
   {
      for (int i = 0; i < imp_.max_fds_; ++i) {
	 if (imp_.fdhandlers_[i]) {
	    i_RemoveCondition(imp_.fdhandlers_[i]);
	 }
      }
   }
#ifndef NDEBUG
   {
      for (int i = 0; i < NSIG; ++i) {
	 assert(imp_.sighandlers_[i] == 0);
      }
   }
   {
      for (int i = 0; i < imp_.max_fds_; ++i) {
	 assert(imp_.fdhandlers_[i] == 0);
      }
   }
#endif
}
