/* $Header$ */

// $Log$
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
#include <limits.h>

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

const LinearExtent LinearExtent::SubExtent_eq(const LinearExtent &extent)
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