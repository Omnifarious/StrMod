#ifndef _LCORE_enum_set_H_  // -*-c++-*-

/*
 * Copyright (C) 2001 Eric M. Hopper <hopper@omnifarious.mn.org>
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

#include <bitset>
#include <string>

#define _LCORE_enum_set_H_

/**
 * \class enum_set enum_set.h LCore/enum_set.h
 *
 * \brief This class is to easily manipulate a set of flags corresponding to
 * members of an enum.
 *
 * \param enum_t The enum type of the flags.
 * \param first The lowest valued enum flag.
 * \param last The highest valued enum flag.
 */
template <class enum_t, enum_t first, enum_t last>
class enum_set : private std::bitset<last - first + 1> {
  private:
   typedef enum_set<enum_t, first, last> self_t;
   //! Used to refer to private parent type to avoid errors in template parameter.
   typedef std::bitset<last - first + 1> parent_t;
   typedef std::bitset<last - first + 1>::reference pref;
  public:
   //! Construct an enum_set with no bits set.
   inline enum_set();
   //! Construct a copy of a different enum_set.
   inline enum_set(const enum_set<enum_t, first, last> &b);
   //! Construct an enum_set with at most one bit set.
   explicit inline enum_set(enum_t val);
   //! Construct an enum_set with at most two bits set.
   inline enum_set(enum_t val1, enum_t val2);
   //! Construct an enum_set with at most three bits set.
   inline enum_set(enum_t val1, enum_t val2, enum_t val3);
   //! Construct an enum_set with at most four bits set.
   inline enum_set(enum_t val1, enum_t val2, enum_t val3, enum_t val4);
   //! Construct an enum_set with at most five bits set.
   inline enum_set(enum_t val1, enum_t val2, enum_t val3, enum_t val4,
                   enum_t val5);
   //! Construct an enum_set with at most six bits set.
   inline enum_set(enum_t val1, enum_t val2, enum_t val3, enum_t val4,
                   enum_t val5, enum_t val6);
   //! Copy an enum_set.
   inline enum_set(self_t &other);

   //! Compute bit1 & bit2 for all corresponding bits in both bitsets.
   inline self_t &operator &=(const self_t &__rhs);
   //! Compute bit1 | bit2 for all corresponding bits in both bitsets.
   inline self_t &operator |=(const self_t &__rhs);
   //! Compute bit1 ^ bit2 for all corresponding bits in both bitsets.
   inline self_t &operator ^=(const self_t &__rhs);
   //! Set a bit
   inline self_t &set(enum_t __pos);
   //! Set a bit to a particular boolean value.
   inline self_t &set(enum_t __pos, bool __val);
   //! Clear all bits.
   inline self_t &reset();
   //! Clear a bit.
   inline self_t &reset(enum_t __pos);
   //! Flip a bit from 0 to 1 or 1 to 0
   inline self_t &flip(enum_t __pos);
   //! Allow bitset[enum_val] syntax.
   inline pref operator[](enum_t __pos);
   //! Allow bitset[enum_val] syntax.
   inline bool operator [](enum_t __pos) const;
   //! How many bits are set?
   size_t count() const                            { return parent_t::count(); }
   //! How many total bits?
   size_t size() const                             { return parent_t::size(); }

   //! Copy one bitset into another
   inline const self_t &operator =(const self_t &b);

   //! Do all corresponding bits match in both bitsets?
   inline bool operator==(const self_t &__rhs) const; 
   //! Do any corresponding bits differ in both bitsets?
   inline bool operator!=(const self_t &__rhs) const;
   //! Is the bit on?
   inline bool test(enum_t __pos) const;
   //! Are any bits set (1, true)?
   bool any() const                                { return parent_t::any(); }
   //! Are all bits clear (0, false)?
   bool none() const                               { return parent_t::none(); }

   //! Return an ASCII representation of the bitset.
   inline std::string to_string() const;

   //! Are any bits set (1, true)?
   operator bool() const                           { return any(); }
};

//-----------------------------inline functions--------------------------------

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set()
{
}

template <class enum_t, enum_t first, enum_t last> inline
enum_set<enum_t, first, last>::enum_set(const enum_set<enum_t, first, last> &b)
     : parent_t(*this)
{
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(enum_t val)
{
   set(val);
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(enum_t val1, enum_t val2)
{
   set(val1);
   set(val2);
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(enum_t val1, enum_t val2,
                                               enum_t val3)
{
   set(val1);
   set(val2);
   set(val3);
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(enum_t val1, enum_t val2,
                                               enum_t val3, enum_t val4)
{
   set(val1);
   set(val2);
   set(val3);
   set(val4);
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(enum_t val1, enum_t val2,
                                               enum_t val3, enum_t val4,
                                               enum_t val5)
{
   set(val1);
   set(val2);
   set(val3);
   set(val4);
   set(val5);
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(enum_t val1, enum_t val2,
                                               enum_t val3, enum_t val4,
                                               enum_t val5, enum_t val6)
{
   set(val1);
   set(val2);
   set(val3);
   set(val4);
   set(val5);
   set(val6);
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>::enum_set(self_t &other)
     : parent_t(*this)
{
}

template <class enum_t, enum_t first, enum_t last>
inline  enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator &=(const self_t &__rhs)
{
   parent_t::operator &=(__rhs);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline  enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator |=(const self_t &__rhs)
{
   parent_t::operator |=(__rhs);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator ^=(const self_t &__rhs)
{
   parent_t::operator ^=(__rhs);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::set(enum_t __pos)
{
   parent_t::set(__pos - first); 
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::set(enum_t __pos, bool __val)
{
   parent_t::set(__pos - first, __val);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::reset()
{
   parent_t::reset();
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::reset(enum_t __pos)
{
   parent_t::reset(__pos - first);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::flip(enum_t __pos)
{
   parent_t::flip(__pos - first);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline typename enum_set<enum_t, first, last>::pref
enum_set<enum_t, first, last>::operator[](enum_t __pos)
{
   return parent_t::operator[](__pos - first);
}

template <class enum_t, enum_t first, enum_t last>
inline bool enum_set<enum_t, first, last>::operator[](enum_t __pos) const
{
   return parent_t::operator [](__pos - first);
}

template <class enum_t, enum_t first, enum_t last>
inline const enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator =(const enum_set<enum_t, first, last> &b)
{
   parent_t::operator =(b);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
inline bool enum_set<enum_t, first, last>::operator==(const self_t &__rhs) const
{
   return parent_t::operator ==(__rhs);
}

template <class enum_t, enum_t first, enum_t last>
inline bool enum_set<enum_t, first, last>::operator!=(const self_t &__rhs) const
{
   return parent_t::operator !=(__rhs);
}

template <class enum_t, enum_t first, enum_t last>
inline bool enum_set<enum_t, first, last>::test(enum_t __pos) const
{
   return parent_t::test(__pos - first);
}

template <class enum_t, enum_t first, enum_t last>
inline std::string enum_set<enum_t, first, last>::to_string() const
{
   std::string fred;
   parent_t::_M_copy_to_string(fred);
   return fred;
}

//--

//! Return bitset containing result of bit1 & bit2 for all corresponding bits.
template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>
operator&(const enum_set<enum_t, first, last> &__x,
          const enum_set<enum_t, first, last> &__y)
{
   enum_set<enum_t, first, last> result(__x);
  result &= __y;
  return result;
}

//! Return bitset containing result of bit1 | bit2 for all corresponding bits.
template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>
operator|(const enum_set<enum_t, first, last> &__x,
          const enum_set<enum_t, first, last> &__y)
{
   enum_set<enum_t, first, last> result(__x);
  result |= __y;
  return result;
}

//! Return bitset containing result of bit1 ^ bit2 for all corresponding bits.
template <class enum_t, enum_t first, enum_t last>
inline enum_set<enum_t, first, last>
operator^(const enum_set<enum_t, first, last> &__x,
          const enum_set<enum_t, first, last> &__y)
{
   enum_set<enum_t, first, last> result(__x);
  result ^= __y;
  return result;
}

#endif
