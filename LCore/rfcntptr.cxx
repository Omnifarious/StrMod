/* $Header$ */

// $Log$
// Revision 1.2  1997/05/12 16:34:53  hopper
// Fixed #include directives so auto-dependency generator would work better.
// --
// Also fixed reference count is decremented to 0 before actual deletion.
// This doesn't fix any current bug, but may prevent future ones in
// multi-threaded environments, and also reduces confusion.
//
// Revision 1.1  1997/05/12 14:32:39  hopper
// Added new RefCountPtr class, and RefCountPtrT class to aid in using
// the ReferenceCounting mixin class.
//

#ifdef __GNUG__
#  pragma implementation "RefCountPtr.h"
#endif

#define _LCORE_RefCountPtr_H_CC
#include "LCore/RefCountPtr.h"
#undef _LCORE_RefCountPtr_H_CC

#include "LCore/RefCounting.h"

const LCore_ClassIdent RefCountPtr::identifier(9UL);

void RefCountPtr::i_SetPtr(ReferenceCounting *p, bool deleteref)
{
   if (p) {
      p->AddReference();
   }
   if (ptr_ && deleteref) {
      if (ptr_->NumReferences() <= 1) {
	 if (ptr_->NumReferences() > 0) {
	    ptr_->DelReference();
	 }
	 delete ptr_;
      } else {
	 ptr_->DelReference();
      }
   }
   ptr_ = p;
}
