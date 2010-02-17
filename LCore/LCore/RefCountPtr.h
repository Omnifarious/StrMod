#ifndef _LCORE_RefCountPtr_H_  // -*-c++-*-

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
   inline void ReleasePtr(bool deleteref = true);

   inline operator bool() const;
   inline bool operator !() const;

   inline const RefCountPtr &operator =(const RefCountPtr &b);
   inline const RefCountPtr &operator =(RC *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual RC *i_CheckType(RC *p) const                { return(p); }
   void i_SetPtr(ReferenceCounting *p, bool deleteref = true);

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

inline void RefCountPtr::ReleasePtr(bool deleteref)
{
   i_SetPtr(0, deleteref);
}

inline RefCountPtr::operator bool() const
{
   return((GetPtr() != 0) ? true : false);
}

inline bool RefCountPtr::operator !() const
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

#if !(defined(_LCORE_RefCountPtr_H_DEBUG) || defined(_LCORE_RefCountPtr_H_CC))
#  if !defined(_LCORE_RefCountPtr_H_NDEBUG_SET)
#     undef NDEBUG
#   else
#     undef _LCORE_RefCountPtr_H_NDEBUG_SET
#   endif
#endif

#endif
