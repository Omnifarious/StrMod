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

// $Revision$

// For a change log, see ChangeLog
//
// Revision 1.3  1996/09/21 18:40:12  hopper
// Changed LinearExtent::SubExtent_eq to return const LinearExtent & to
// be more like x= operators.
//
// Revision 1.2  1996/06/29 06:42:21  hopper
// Added const qualifier.
// New function SubExtent_eq, in place SubExtent calc.
//
// Revision 1.1  1996/05/08 11:16:47  hopper
// First functional revision
//

#ifdef __GNUG__
#  pragma implementation "LinearExtent.h"
#endif

#include <StrMod/LinearExtent.h>
#include <climits>

namespace strmod {
namespace strmod {

const LinearExtent LinearExtent::full_extent(0, UINT_MAX);

const LinearExtent LinearExtent::SubExtent(const LinearExtent &extent) const
{
   if (extent.m_offset > m_length) {
      return(LinearExtent(m_offset + m_length, 0));
   } else {
      off_t newoff = m_offset + extent.m_offset;

      if ((extent.m_offset + extent.m_length) > m_length) {
	 return(LinearExtent(newoff, m_length - extent.m_offset));
      } else {
	 return(LinearExtent(newoff, extent.m_length));
      }
   }
}

const LinearExtent &LinearExtent::SubExtent_eq(const LinearExtent &extent)
{
   if (this == &extent)
   {
      *this = SubExtent(extent);
   }
   else
   {
      if (extent.m_offset > m_length) {
	 m_offset += m_length;
	 m_length = 0;
      } else {
	 m_offset += extent.m_offset;
	 if ((extent.m_offset + extent.m_length) > m_length) {
	    m_length -= extent.m_offset;
	 } else {
	    m_length = extent.m_length;
	 }
      }
   }
   return(*this);
}

void LinearExtent::LengthenLeft(length_t by)
{
   if (m_offset < by) {
      by = m_offset;
   }

   m_length += by;
   m_offset -= by;
}

void LinearExtent::LengthenCenter(length_t by)
{
   off_t off_adj = by / 2;

   m_length += by;
   if (m_offset > off_adj) {
      m_offset -= off_adj;
   } else {
      m_offset = 0;
   }
}

void LinearExtent::ShortenLeft(length_t by)
{
   if (m_length < by) {
      by = m_length;
   }

   m_length -= by;
   m_offset += by;
}

void LinearExtent::ShortenCenter(length_t by)
{
   if (m_length < by) {
      by = m_length;
   }

   off_t off_adj = by / 2;

   m_offset += off_adj;
   m_length -= by;
}

};  // End namespace strmod
};  // End namespace strmod
