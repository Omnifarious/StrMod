#ifndef _LCORE_RefCountPtrT_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
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
class RefCountPtrT : virtual public RefCountPtr {
 public:
   RefCountPtrT(T *rfptr = 0) : RefCountPtr(rfptr)                          { }
   RefCountPtrT(const RefCountPtrT<T> &b) : RefCountPtr(b)                  { }

   inline T &operator *() const;
   inline T *operator ->() const;

   inline T *GetPtr() const;

   inline const RefCountPtr &operator =(const RefCountPtr &b);
   inline const RefCountPtr &operator =(RC *b);
   inline const RefCountPtrT<T> &operator =(const RefCountPtrT<T> &b);
   inline const RefCountPtrT<T> &operator =(T *b);

 protected:
   inline virtual RC *i_CheckType(RC *p) const;
};

//-----------------------------inline functions--------------------------------

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
inline const RefCountPtr &RefCountPtrT<T>::operator =(const RefCountPtr &b)
{
   return(RefCountPtr::operator =(b));
}

template <class T>
inline const RefCountPtr &RefCountPtrT<T>::operator =(ReferenceCounting *b)
{
   return(RefCountPtr::operator =(b));
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
