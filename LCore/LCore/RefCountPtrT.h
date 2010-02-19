#ifndef _LCORE_RefCountPtrT_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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

#define _LCORE_RefCountPtrT_H_

namespace strmod {
namespace lcore {

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
   inline virtual ReferenceCounting *i_CheckType(ReferenceCounting *p) const;
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

} // namespace lcore
} // namespace strmod

#endif
