#ifndef _STR_LinearExtent_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog
//
// Revision 1.4  1999/09/16 01:37:17  hopper
// Added this function to help write the telnet code.
//
// Revision 1.3  1996/09/21 18:40:04  hopper
// Changed LinearExtent::SubExtent_eq to return const LinearExtent & to
// be more like x= operators.
//
// Revision 1.2  1996/06/29 06:24:25  hopper
// Added const in places that needed it.
// Added new SubExtent_eq for in-place sub-extension calculation.
// Added declaration for stream output operator.
//
// Revision 1.1  1996/05/08 11:16:53  hopper
// First functional revision
//

#define _STR_LinearExtent_H_

namespace std {
class ostream;
};

namespace strmod {
namespace strmod {

/** \class LinearExtent LinearExtent.h StrMod/LinearExtent.h
 * A simple class describing a subrange of a contiguous sequence.
 *
 * This class sees such a subrange as beginning at an offset, and proceeding
 * for some number of items.
 */
class LinearExtent {
 public:
   typedef unsigned int off_t;  ///< Type for expressing the offset.
   typedef unsigned int length_t;  ///< Type for expressing the length.

   /**
    * A constant for an infinite length extent starting at offset 0.
    *
    * Not actually _infinite_, but of length UINT_MAX.
    */
   static const LinearExtent full_extent;

   //! A zero length extent starting at offset 0.
   inline LinearExtent();
   //! An extent starting at \c offset having length \c length.
   inline LinearExtent(off_t offset, length_t length);
   //! Not meant to be inherited from, so not virtual.
   inline ~LinearExtent();

   //! Retrieve offset.
   inline off_t Offset() const;
   //! Set offset.
   inline void Offset(off_t new_off);
   //! Retrieve length.
   inline length_t Length() const;
   //! Set length.
   inline void Length(length_t new_length);

   /** \name STL Transforms
    * These functions translate the extent coordinate system of offset, length
    * into the STL coordinate system of [begin, end).
    */
   //@{
   //! Offset of beginning of extent (for STL like algorithms).  Same as
   //! Offset().
   inline off_t beginOffset() const;
   //! Offset of end of extent (for STL like algorithms).  Same as Offset() +
   //! Length()
   inline off_t endOffset() const;
   //@}

   //! Given two extents, what is extent is common to both?
   inline const LinearExtent intersection(const LinearExtent &other) const;

   /** \name Extent as a Moveable Tape Measure
    * To understand what all these functions do, it's helpful to keep an image
    * of the full contiguous sequence in mind, whith the LinearExtent acting
    * as a sort of tape measure stretched along some part of the sequence.
    */
   //@{
   //! Lengthen an extent by lowering the offset and increasing the length by
   //! the same amount.
   void LengthenLeft(length_t by);
   //! Lengthen an extent by lowering the offset by half of \c by and
   //! increasing the length by \c by.
   void LengthenCenter(length_t by);
   //! Lengthen an extent merely by adding to the length.
   inline void LengthenRight(length_t by);

   //! Shorten an extent by increasing its offset and decreasing its length by
   //! the same amount.
   void ShortenLeft(length_t by);
   //! Shorten an extent by increasing its offset by half of \c by and
   //! decreasing its length by \c by.
   void ShortenCenter(length_t by);
   //! Shorten an extent by simply decreasing its length.
   inline void ShortenRight(length_t by);

   //! Add to the offset.
   inline void MoveRight(off_t by);
   //! Subtract from the offset.
   inline void MoveLeft(off_t by);
   //@}

   //! Translate a subextent into the underlying sequence's coordinate space.
   const LinearExtent SubExtent(const LinearExtent &extent) const;
   //! The function is to SubExtent as += is to +.
   const LinearExtent &SubExtent_eq(const LinearExtent &extent);

 private:
   off_t m_offset;
   length_t m_length;
};

std::ostream &operator <<(std::ostream &os, const LinearExtent &ext);

//-----------------------------inline functions--------------------------------

inline LinearExtent::LinearExtent() : m_offset(0), m_length(0)
{
}

inline LinearExtent::LinearExtent(off_t offset, length_t length)
     : m_offset(offset), m_length(length)
{
}

inline LinearExtent::~LinearExtent()
{
}

inline LinearExtent::off_t LinearExtent::Offset() const
{
   return(m_offset);
}

inline void LinearExtent::Offset(LinearExtent::off_t new_off)
{
   m_offset = new_off;
}

inline LinearExtent::length_t LinearExtent::Length() const
{
   return(m_length);
}

inline void LinearExtent::Length(LinearExtent::length_t new_length)
{
   m_length = new_length;
}

inline LinearExtent::off_t LinearExtent::beginOffset() const
{
    return(Offset());
}

inline LinearExtent::off_t LinearExtent::endOffset() const
{
    return(Offset() + Length());
}

inline const LinearExtent
LinearExtent::intersection(const LinearExtent &other) const
{
   off_t intstart = (Offset() > other.Offset()) ? Offset() : other.Offset();
   off_t myend = endOffset();
   off_t otherend = other.endOffset();
   off_t intend = (myend < otherend) ? myend : otherend;
   return(LinearExtent(intstart,
		       (intend <= intstart) ? 0 : (intend - intstart)));
}

inline void LinearExtent::MoveRight(LinearExtent::off_t by)
{
   m_offset += by;
}

inline void LinearExtent::MoveLeft(LinearExtent::off_t by)
{
   if (by < m_offset) {
      m_offset -= by;
   } else {
      m_offset = 0;
   }
}

inline void LinearExtent::LengthenRight(length_t by)
{
   m_length += by;
}

inline void LinearExtent::ShortenRight(length_t by)
{
   if (m_length > by) {
      m_length -= by;
   } else {
      m_length = 0;
   }
}

};  // namespace strmod
};  // namespace strmod

#endif
