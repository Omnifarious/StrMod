#ifndef _STR_LinearExtent_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/05/08 11:16:53  hopper
// First functional revision
//

#define _STR_LinearExtent_H_

class LinearExtent {
 public:
   typedef unsigned int off_t;
   typedef unsigned int length_t;

   inline LinearExtent();
   inline LinearExtent(off_t offset, length_t length);
   inline ~LinearExtent();

   inline off_t Offset() const;
   inline void Offset(off_t new_off);
   inline length_t Length();
   inline void Length(length_t new_length);

   void LengthenLeft(length_t by);
   void LengthenCenter(length_t by);
   inline void LengthenRight(length_t by);

   void ShortenLeft(length_t by);
   void ShortenCenter(length_t by);
   inline void ShortenRight(length_t by);

   inline void MoveRight(off_t by);
   inline void MoveLeft(off_t by);

   const LinearExtent SubExtent(const LinearExtent &extent);

 private:
   off_t m_offset;
   length_t m_length;

};

//-----------------------------inline functions--------------------------------

LinearExtent::LinearExtent() : m_offset(0), m_length(0)
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

inline LinearExtent::length_t LinearExtent::Length()
{
   return(m_length);
}

inline void LinearExtent::Length(LinearExtent::length_t new_length)
{
   m_length = new_length;
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

#endif
