/* $Header$ */

// $Log$
// Revision 1.1  1997/05/12 14:32:39  hopper
// Added new RefCountPtr class, and RefCountPtrT class to aid in using
// the ReferenceCounting mixin class.
//

#define _LCORE_RefCountPtr_H_CC
#include <LCore/RefCountPtr.h>
#undef _LCORE_RefCountPtr_H_CC

#include <LCore/RefCounting.h>

const LCore_ClassIdent RefCountPtr::identifier(9UL);

void RefCountPtr::i_SetPtr(ReferenceCounting *p, bool deleteref)
{
   if (p) {
      p->AddReference();
   }
   if (ptr_ && deleteref) {
      if (ptr_->NumReferences() <= 1) {
	 delete ptr_;
      } else {
	 ptr_->DelReference();
      }
   }
   ptr_ = p;
}
