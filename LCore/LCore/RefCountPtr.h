#ifndef _LCORE_RefCountPtr_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog
//
// Revision 1.3  1998/05/01 11:59:32  hopper
// Changed bool to bool_val or bool_cst as appropriate so it will be easier to
// port to platforms that don't support bool.
//
// Revision 1.2  1997/05/12 16:28:33  hopper
// Fixed operator = to only check type of non-NULL pointers.
//
// Revision 1.1  1997/05/12 14:32:55  hopper
// Added new RefCountPtr class, and RefCountPtrT class to aid in using
// the ReferenceCounting mixin class.
//

#if !(defined(_LCORE_RefCountPtr_H_DEBUG) || defined(_LCORE_RefCountPtr_H_CC))
#  ifndef NDEBUG
#     define NDEBUG
#  else
#     define _LCORE_RefCountPtr_H_NDEBUG_SET
#  endif
#endif

#ifndef _LCORE_Protocol_H_
#  include <LCore/Protocol.h>
#endif
#include <bool.h>
#include <assert.h>

#define _LCORE_RefCountPtr_H_

class ReferenceCounting;

class RefCountPtr : virtual public Protocol {
 public:
   typedef ReferenceCounting RC;
   static const LCore_ClassIdent identifier;

   inline RefCountPtr();
   inline RefCountPtr(RC *rfptr);
   inline RefCountPtr(const RefCountPtr &b);
   inline virtual ~RefCountPtr();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline RC &operator *() const;
   inline RC *operator ->() const;

   inline RC *GetPtr() const                           { return(ptr_); }
   inline void ReleasePtr(bool_val deleteref = true);

   inline operator bool_cst() const;
   inline bool_val operator !() const;

   inline const RefCountPtr &operator =(const RefCountPtr &b);
   inline const RefCountPtr &operator =(RC *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual RC *i_CheckType(RC *p) const                { return(p); }
   void i_SetPtr(RC *p, bool_val deleteref = true);

 private:
   RC *ptr_;
};

//-----------------------------inline functions--------------------------------

inline RefCountPtr::RefCountPtr() : ptr_(0)
{
}

inline RefCountPtr::RefCountPtr(RC *rfptr) : ptr_(0)
{
   if (rfptr) {
      i_SetPtr(rfptr);
   }
}

inline RefCountPtr::RefCountPtr(const RefCountPtr &b) : ptr_(0)
{
   if (b) {
      i_SetPtr(b.GetPtr());
   }
}

inline RefCountPtr::~RefCountPtr()
{
   ReleasePtr(true);
}

inline int RefCountPtr::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#if defined(NDEBUG) || defined(_LCORE_RefCountPtr_H_CC)
inline ReferenceCounting &RefCountPtr::operator *() const
{
   assert(GetPtr() != 0);

   return(*GetPtr());
}

inline ReferenceCounting *RefCountPtr::operator ->() const
{
   assert(GetPtr() != 0);

   return(GetPtr());
}
#endif

inline void RefCountPtr::ReleasePtr(bool_val deleteref)
{
   i_SetPtr(0, deleteref);
}

inline RefCountPtr::operator bool_cst() const
{
   return((GetPtr() != 0) ? true : false);
}

inline bool_val RefCountPtr::operator !() const
{
   return(GetPtr() == 0);
}

inline const RefCountPtr &RefCountPtr::operator =(const RefCountPtr &b)
{
   if (this != &b) {
      RC *p = b.GetPtr();

      operator =(p);
   }
   return(*this);
}

inline const RefCountPtr &RefCountPtr::operator =(RC *b)
{
   if (GetPtr() != b) {
      if (b) {
	 b = i_CheckType(b);
      }
      i_SetPtr(b);
   }
   return(*this);
}

#endif
