#ifndef _LCORE_RefCountPtrT_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog
//
// Revision 1.2  1997/05/12 14:36:03  hopper
// Removed dangerous two dangerous operator =.
//
// Revision 1.1  1997/05/12 14:32:55  hopper
// Added new RefCountPtr class, and RefCountPtrT class to aid in using
// the ReferenceCounting mixin class.
//

#ifndef _LCORE_Protocol_H_
#  include <LCore/Protocol.h>
#endif
#ifndef _EH_0_RefCounting_H_
#  include <LCore/RefCounting.h>
#endif
#ifndef _LCORE_RefCountPtr_H_
#  include <LCore/RefCountPtr.h>
#endif
#include <bool.h>

#define _LCORE_RefCountPtrT_H_

template <class T>
class RefCountPtrT : public RefCountPtr {
 public:
   RefCountPtrT(const RefCountPtrT<T> &b) : RefCountPtr(b)                  { }
   inline RefCountPtrT(T *rfptr = 0);

   inline T &operator *() const;
   inline T *operator ->() const;

   inline T *GetPtr() const;

   inline const RefCountPtrT<T> &operator =(const RefCountPtrT<T> &b);
   inline const RefCountPtrT<T> &operator =(const RefCountPtr &b);
   inline const RefCountPtrT<T> &operator =(T *b);

 protected:
   inline virtual RC *i_CheckType(RC *p) const;
};

//-----------------------------inline functions--------------------------------

template <class T>
inline RefCountPtrT<T>::RefCountPtrT(T *rfptr) : RefCountPtr(rfptr)
{
}

template <class T>
inline T &RefCountPtrT<T>::operator *() const
{
   return(*GetPtr());
}

template <class T>
inline T *RefCountPtrT<T>::operator ->() const
{
   return(GetPtr());
}

template <class T>
inline T *RefCountPtrT<T>::GetPtr() const
{
   return(static_cast<T *>(RefCountPtr::GetPtr()));
}

template <class T>
inline const RefCountPtrT<T> &
RefCountPtrT<T>::operator =(const RefCountPtrT<T> &b)
{
   RefCountPtr::operator =(b);
   return(*this);
}

template <class T>
inline const RefCountPtrT<T> &
RefCountPtrT<T>::operator =(const RefCountPtr &b)
{
   RefCountPtr::operator =(b);
   return(*this);
}

template <class T>
inline const RefCountPtrT<T> &
RefCountPtrT<T>::operator =(T *b)
{
   RefCountPtr::operator =(b);
   return(*this);
}

template <class T>
inline ReferenceCounting *
RefCountPtrT<T>::i_CheckType(ReferenceCounting *p) const
{
   return(p->AreYouA(T::identifier) ? p : 0);
}

#endif
