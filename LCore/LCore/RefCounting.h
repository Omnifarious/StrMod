#ifndef _EH_0_RefCounting_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog
//
// Revision 1.2  1997/05/12 16:27:53  hopper
// Fixed ReferenceCounting::NumReferences to be a const method, as befits
// an accessor.
//
// Revision 1.1  1996/03/26 16:23:02  hopper
// Moved ReferenceCounting class over from Container library.
// Fixed up class numbering system so LCore uses a seperate library number
// from the container library.
//
// Revision 1.2  1996/02/22 03:56:03  hopper
// Fixed to be close to the new 'standard' class format.
// Fixed to be less open to subclasses.
//
// Revision 1.1.1.1  1996/02/20 02:32:52  hopper
// Imported into CVS
//
// Revision 0.2  1994/10/30  04:49:32  hopper
// Moved various things into the new LCore library.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#ifdef __GNUG__
#  pragma interface
#endif

#ifndef _LCORE_Protocol_H_
#  include <LCore/Protocol.h>
#endif

#define _EH_0_RefCounting_H_

namespace strmod {
namespace lcore {

/** \class ReferenceCounting RefCounting.h LCore/RefCounting.h
 * A base mixin class for reference counted things.
 *
 * There are many ways to handle reference counting as a general garbage
 * collection technique in C++.  This helps implement one of the more efficient,
 * but also somewhat invasive ones.  This one has the reference count stored
 * inside the class itself.  This means the class holds data that really doesn't
 * have much to do with the class, but some classes are designed to be used in
 * ways where you know reference counting will be needed and useful as a garbage
 * collection technique.
 *
 * This class has an associated smart pointer class, RefCountPtr, that can be
 * used to point at instances of this class.  The smart pointer class will track
 * references for you.  But, there is no requirement to use it if you want to do
 * your reference counting in some other way.
*/
class ReferenceCounting : virtual public Protocol
{
 public:
   static const LCore_ClassIdent identifier;

   //! Initialize with a reference count
   ReferenceCounting(U4Byte refs) : refcounter(refs)        {}
   //! Not much to do on destruction
   virtual ~ReferenceCounting()                             {}

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || Protocol::AreYouA(cid));
   }

   //! Add a reference to the count.
   void AddReference()                      { refcounter++; }
   /** Remove a reference from the count and stop the counter from rolling over backwards
    *
    * Note that this class does not consider it it's job to actually delete
    * itself when the reference count goes to 0.  That job is more properly left
    * to the smart pointer class.
    */
   void DelReference()                      { if (refcounter) refcounter--; }
   //! How many references?
   U4Byte NumReferences() const             { return(refcounter); }

 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

   //! Add more than 1 reference
   void AddReferences(U4Byte num)           { refcounter += num; }
   //! Remove more than 1 reference, again with protection from rolling over backwards.
   inline void DelReferences(U4Byte num);

 private:
   mutable U4Byte refcounter;
};

inline const ClassIdent *ReferenceCounting::i_GetIdent() const
{
   return(&identifier);
}

inline void ReferenceCounting::DelReferences(U4Byte num)
{
   if (refcounter > num)
      refcounter -= num;
   else
      refcounter = 0;
}

} // namespace lcore
} // namespace strmod

#endif
