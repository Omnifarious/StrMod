#ifndef _LCORE_RefCountPtr_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/*
 * Copyright 1991-2002 Eric M. Hopper <hopper@omnifarious.org>
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

#ifndef _LCORE_Protocol_H_
#  include <LCore/Protocol.h>
#endif
#include <cassert>

#define _LCORE_RefCountPtr_H_

namespace strmod {
namespace lcore {

class ReferenceCounting;

/** \class RefCountPtr RefCountPtr.h LCore/RefCountPtr.h
 * A smart pointer class that points to objects of type ReferenceCounting and
 * maintains their reference counts.
 *
 * There's not much more to this than the brief description says.
*/
class RefCountPtr : virtual public Protocol
{
 public:
   typedef ReferenceCounting RC;
   static const LCore_ClassIdent identifier;

   //! Contruct a null pointer
   inline RefCountPtr();
   //! Construct a pointer pointing at rfptr and increment its reference count
   inline RefCountPtr(RC *rfptr);
   //! Copy a RefCountPtr and incremement the reference count of the thing pointed to
   inline RefCountPtr(const RefCountPtr &b);
   /** Destroy the pointer and decrement the reference count of the thing pointed to.
    *
    * If the reference count of the thing pointed to goes to 0, its destructor
    * will be called.
    */
   inline virtual ~RefCountPtr();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //! Smart pointer operator, if _LCORE_RefCountPtr_H_DEBUG is set, asserts non-null
   inline RC &operator *() const;
   //! Smart pointer operator, if _LCORE_RefCountPtr_H_DEBUG is set, asserts non-null
   inline RC *operator ->() const;

   //! What am I pointing at?
   inline RC *GetPtr() const                           { return(ptr_); }
   /** Set me to point at null, decrement the reference count of previous pointer destination
    *
    * If the reference count of the thing previously pointed to goes to 0, its
    * destructor will be called.
    */
   inline void ReleasePtr(bool deleteref = true);

   //! Am I non-null?
   inline operator bool() const;
   //! Am I null?
   inline bool operator !() const;

   /** Copy a reference counted pointer, handling reference counts appropriately
    *
    * This handles *this = *this, and *this = ReferenceCounting(*this) properly.
    * It decrements the reference count of, and possibly destroys what's
    * currently being pointed at, and increments the reference count of what
    * will be pointed to.
    *
    * This also uses the i_CheckType() function to do some type checking of the
    * pointer being pointed to.  This is so new pointer classes with type
    * constraints can be derived from this class.
    */
   inline const RefCountPtr &operator =(const RefCountPtr &b);
   /** Copy from a regular pointer, handling reference counts appropriately
    *
    * This handles *this = this->GetPtr() properly.  It decrements the reference
    * count of, and possibly destroys what's currently being pointed at, and
    * increments the reference count of what will be pointed to.
    *
    * This also uses the i_CheckType() function to do some type checking of the
    * pointer being pointed to.  This is so new pointer classes with type
    * constraints can be derived from this class.
    */
   inline const RefCountPtr &operator =(RC *b);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   /** Do some type checking of a ReferenceCounting pointer to make sure it points at the right type.
    *
    * @param p The pointer who's type needs to be checked.
    * @return null if the pointer is the wrong type, the value of p if it is of the right type.
    */
   virtual RC *i_CheckType(RC *p) const                { return(p); }
   /** Change what this RefCountPtr points to, maybe modifying reference counts
    *
    * This will properly handle i_SetPtr(GetPtr()). Normally, it will increment
    * the reference count of the new value of the pointer, then decrement the
    * reference count of the old value, possibly deleting the pointed to object
    * if the reference count goes to 0.  If deleteref is false, the decrement
    * won't happen.
    *
    * @param p The new value of the pointer.
    * @param deleteref Should the old value's reference count be decremented?
    */
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

#if !defined(_LCORE_RefCountPtr_H_DEBUG) || defined(_LCORE_RefCountPtr_H_CC)
inline ReferenceCounting &RefCountPtr::operator *() const
{
#ifdef _LCORE_RefCountPtr_H_CC
   assert(GetPtr() != 0);
#endif

   return *GetPtr();
}

inline ReferenceCounting *RefCountPtr::operator ->() const
{
#ifdef _LCORE_RefCountPtr_H_CC
   assert(GetPtr() != 0);
#endif

   return GetPtr();
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

} // namespace lcore
} // namespace strmod

#endif
