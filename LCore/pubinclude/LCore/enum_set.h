#ifndef _LCORE_enum_set_H_  // -*-c++-*-

/*
 * Copyright 2001-2010 Eric M. Hopper <hopper@omnifarious.org>
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

#include <LCore/simple_bitset.h>
#include <string>
#include <initializer_list>

#define _LCORE_enum_set_H_

namespace strmod {
namespace lcore {

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
//class enum_set : private std::bitset<last - first + 1>
class enum_set : private simple_bitset<last - first + 1>
{
  private:
   typedef enum_set<enum_t, first, last> self_t;
   //! Used to refer to private parent type to avoid errors in template
   //! parameter.
   typedef simple_bitset<last - first + 1> parent_t;
//     typedef simple_bitset<last - first + 1>::reference pref;
  public:
   //! Construct an enum_set with no bits set.
   constexpr enum_set() = default;

   //! Construct an enum_set with all the bits specified in the initializer
   //! list set.
   constexpr enum_set(::std::initializer_list<enum_t> vals);

   //! Copy an enum_set.
   constexpr enum_set(const self_t &other) = default;

   //! Compute bit1 & bit2 for all corresponding bits in both bitsets.
   constexpr self_t &operator &=(const self_t &rhs);
   //! Compute bit1 | bit2 for all corresponding bits in both bitsets.
   constexpr self_t &operator |=(const self_t &rhs);
   //! Compute bit1 ^ bit2 for all corresponding bits in both bitsets.
   constexpr self_t &operator ^=(const self_t &rhs);
   //! Set a bit
   constexpr self_t &set(enum_t pos);
   //! Set a bit to a particular boolean value.
   constexpr self_t &set(enum_t pos, bool val);
   //! Clear all bits.
   constexpr self_t &reset();
   //! Clear a bit.
   constexpr self_t &reset(enum_t pos);
   //! Flip a bit from 0 to 1 or 1 to 0
   constexpr self_t &flip(enum_t pos);
//     //! Allow bitset[enum_val] syntax.
//     inline pref operator[](enum_t1pos);
   //! Allow bitset[enum_val] syntax.
   [[nodiscard]] constexpr bool operator [](enum_t pos) const;
   //! How many bits are set?
   [[nodiscard]] constexpr size_t count() const    { return parent_t::count(); }
   //! How many total bits?
   [[nodiscard]] constexpr size_t size() const     { return parent_t::size(); }

   //! Copy one bitset into another
   constexpr self_t &operator =(const self_t &b) = default;

   //! Do all corresponding bits match in both bitsets?
   constexpr bool operator==(const self_t &rhs) const;
   //! Do any corresponding bits differ in both bitsets?
   constexpr bool operator!=(const self_t &rhs) const;
   //! Is the bit on?
   [[nodiscard]] constexpr bool test(enum_t pos) const;
   //! Are any bits set (1, true)?
   [[nodiscard]] constexpr bool any() const        { return parent_t::any(); }
   //! Are all bits clear (0, false)?
   [[nodiscard]] bool none() const                 { return parent_t::none(); }

   // Use the parent's
   // inline std::string to_string() const override;

   //! Are any bits set (1, true)?
   explicit operator bool() const                  { return any(); }
};

//-----------------------------inline functions--------------------------------

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last>::enum_set(
        ::std::initializer_list<enum_t> vals
)
{
   for (auto const &val: vals) {
      set(val);
   }
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator &=(const self_t &rhs)
{
   parent_t::operator &=(rhs);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator |=(const self_t &rhs)
{
   parent_t::operator |=(rhs);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::operator ^=(const self_t &rhs)
{
   parent_t::operator ^=(rhs);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::set(enum_t pos)
{
   parent_t::set(pos - first);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::set(enum_t pos, bool val)
{
   parent_t::set(pos - first, val);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::reset()
{
   parent_t::reset();
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::reset(enum_t pos)
{
   parent_t::reset(pos - first);
   return *this;
}

template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last> &
enum_set<enum_t, first, last>::flip(enum_t pos)
{
   parent_t::flip(pos - first);
   return *this;
}

//  template <class enum_t, enum_t first, enum_t last>
//  inline typename enum_set<enum_t, first, last>::pref
//  enum_set<enum_t, first, last>::operator[](enum_t pos)
//  {
//     return parent_t::operator[](pos - first);
//  }

template <class enum_t, enum_t first, enum_t last>
constexpr bool enum_set<enum_t, first, last>::operator[](enum_t pos) const
{
   return parent_t::operator [](pos - first);
}

template <class enum_t, enum_t first, enum_t last>
constexpr bool enum_set<enum_t, first, last>::operator==(const self_t &rhs) const
{
   return parent_t::operator ==(rhs);
}

template <class enum_t, enum_t first, enum_t last>
constexpr bool enum_set<enum_t, first, last>::operator!=(const self_t &rhs) const
{
   return parent_t::operator !=(rhs);
}

template <class enum_t, enum_t first, enum_t last>
constexpr bool enum_set<enum_t, first, last>::test(enum_t pos) const
{
   return parent_t::test(pos - first);
}

//--

//! Return bitset containing result of bit1 & bit2 for all corresponding bits.
template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last>
operator&(const enum_set<enum_t, first, last> &x,
          const enum_set<enum_t, first, last> &y)
{
   enum_set<enum_t, first, last> result(x);
   result &= y;
   return result;
}

//! Return bitset containing result of bit1 | bit2 for all corresponding bits.
template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last>
operator|(const enum_set<enum_t, first, last> &x,
          const enum_set<enum_t, first, last> &y)
{
   enum_set<enum_t, first, last> result(x);
   result |= y;
   return result;
}

//! Return bitset containing result of bit1 ^ bit2 for all corresponding bits.
template <class enum_t, enum_t first, enum_t last>
constexpr enum_set<enum_t, first, last>
operator^(const enum_set<enum_t, first, last> &x,
          const enum_set<enum_t, first, last> &y)
{
   enum_set<enum_t, first, last> result(x);
   result ^= y;
   return result;
}

} // namespace lcore
} // namespace strmod

#endif
