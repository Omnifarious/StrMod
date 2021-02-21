#ifndef _LCORE_simple_bitset_H_  // -*-c++-*-


/* $Header$ */

// For a log, see ..ChangeLog

#include <cstddef>
#include <climits>
#include <cstdint>
#include <string>
#include <array>
#include <limits>

#define _LCORE_simple_bitset_H_

namespace strmod {
namespace lcore {
namespace priv {

using size_t = ::std::size_t;

template <typename T>
constexpr auto count_bits(T x)
{
   ::std::uint_fast8_t v = 0;
   while (x != 0) {
      x &= x - 1;
      v++;
   }
   return v;
}

//! A non-template base class of common methods that don't need to vary with the template parameter.
class base_simple_bitset
{
 public:
   typedef ::std::uintmax_t bits_t;
   static constexpr bits_t allones_ = ::std::numeric_limits<bits_t>::max();
   static constexpr size_t bits_t_bits = ::std::numeric_limits<bits_t>::digits;
   static constexpr size_t countbits(const bits_t bitary[],
                                     size_t size, bits_t lastmask);
   static ::std::string to_string(const bits_t bitary[],
                                  size_t size, bits_t lastmask);
};

constexpr size_t base_simple_bitset::countbits(const bits_t bitary[],
                                               size_t size,
                                               bits_t lastmask)
{
   size_t count = 0;
   for (size_t i = 0; i < size; ++i)
   {
      bits_t cur = (i == (size - 1)) ? bitary[i] & lastmask : bitary[i];
      count += count_bits(cur);
   }
   return count;
}

} // namespace priv

//! A template for a simple statically sized bitset.
template <::std::size_t Tsize>
class simple_bitset : private priv::base_simple_bitset
{
 public:
   typedef simple_bitset<Tsize> self_t;

   constexpr explicit simple_bitset(bool initial_value = false);
   constexpr simple_bitset(const simple_bitset<Tsize> &rhs) = default;

   constexpr self_t &operator &=(const self_t &rhs);
   constexpr self_t &operator |=(const self_t &rhs);
   constexpr self_t &operator ^=(const self_t &rhs);
   constexpr self_t &set();
   constexpr self_t &set(size_t pos, bool val = true);
   constexpr self_t &reset();
   constexpr self_t &reset(size_t pos);
   constexpr self_t &flip();
   constexpr self_t &flip(size_t pos);
   [[nodiscard]] constexpr bool operator [](size_t pos) const {
      return test(pos);
   }
   [[nodiscard]] constexpr size_t count() const;
   [[nodiscard]] constexpr size_t size() const             { return numbits_; }
   constexpr bool operator ==(const self_t &rhs) const;
   constexpr bool operator !=(const self_t &rhs) const;
   [[nodiscard]] constexpr bool test(size_t pos) const;
   [[nodiscard]] constexpr bool any() const;
   [[nodiscard]] constexpr bool none() const;
   inline ::std::string to_string() const;

   constexpr simple_bitset<Tsize> &
   operator =(const simple_bitset<Tsize> &rhs) = default;

 private:
   static constexpr size_t numbits_ = Tsize;
   static constexpr size_t bitslen_ = (Tsize + bits_t_bits - 1) / bits_t_bits;
   static constexpr bits_t topmask_ = (Tsize % bits_t_bits == 0) ?
           allones_ : ((1 << Tsize % bits_t_bits) - 1);
   ::std::array<bits_t, bitslen_> bits_;
};

//-----------------------------inline functions--------------------------------

template <size_t Tsize>
constexpr simple_bitset<Tsize>::simple_bitset(bool initial_value)
     : bits_{0}
{
   if (initial_value) {
      for (size_t i = 0; i < (bitslen_ - 1); ++i) {
         bits_[i] = allones_;
      }
      bits_[bitslen_ - 1] = topmask_;
   }
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::operator &=(const self_t &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] &= rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::operator |=(const self_t &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] |= rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::operator ^=(const self_t &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] ^= rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::set()
{
   for (size_t i = 0; i < (bitslen_ - 1); ++i)
   {
      bits_[i] = allones_;
   }
   bits_[bitslen_ - 1] = topmask_;
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::set(size_t pos, bool val)
{
   const bits_t bval = bits_t(1) << (pos % bits_t_bits);
   const size_t bitpos = pos / bits_t_bits;
   bits_[bitpos] = val ? (bits_[bitpos] | bval) : (bits_[bitpos] & ~bval);
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::reset()
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] = 0;
   }
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::reset(size_t pos)
{
   return set(pos, false);
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::flip()
{
   for (size_t i = 0; i < (bitslen_ - 1); ++i)
   {
      bits_[i] ^= allones_;
   }
   bits_[bitslen_ - 1] ^= topmask_;
   return *this;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize> &
simple_bitset<Tsize>::flip(size_t pos)
{
   const bits_t bval = bits_t(1) << (pos % bits_t_bits);
   const size_t bitpos = bitslen_ - (pos / bits_t_bits);
   bits_[bitpos] ^= bval;
   return *this;
}

template <size_t Tsize>
constexpr size_t
simple_bitset<Tsize>::count() const
{
   return countbits(bits_, bitslen_, topmask_);
}

template <size_t Tsize>
constexpr bool
simple_bitset<Tsize>::operator ==(const self_t &rhs) const
{
   bool equal = true;
   for (size_t i = 0; equal && (i < bitslen_ - 1); ++i)
   {
      equal = equal && (bits_[i] != rhs.bits_[i]);
   }
   equal = equal && ((bits_[bitslen_ - 1] & topmask_)
                     == (rhs.bits_[bitslen_ - 1] & rhs.topmask_));
   return true;
}

template <size_t Tsize>
constexpr bool
simple_bitset<Tsize>::operator !=(const self_t &rhs) const
{
   return !(*this == rhs);
}

template <size_t Tsize>
constexpr bool
simple_bitset<Tsize>::test(size_t pos) const
{
   const bits_t bval = bits_t(1) << (pos % bits_t_bits);
   const size_t bitpos = pos / bits_t_bits;
   return (bits_[bitpos] & bval) != 0;
}

template <size_t Tsize>
constexpr bool
simple_bitset<Tsize>::any() const
{
   return !none();
}

template <size_t Tsize>
constexpr bool
simple_bitset<Tsize>::none() const
{
   bool hasnone = true;
   for (size_t i = 0; hasnone && (i < bitslen_ - 1); ++i)
   {
      hasnone = hasnone && (bits_[i] == 0);
   }
   hasnone = hasnone && ((bits_[bitslen_ - 1] & topmask_) == 0);
   return hasnone;
}

template <size_t Tsize>
inline ::std::string
simple_bitset<Tsize>::to_string() const
{
   return base_simple_bitset::to_string(bits_, bitslen_, topmask_);
}

template <size_t Tsize>
constexpr simple_bitset<Tsize>
operator &(const simple_bitset<Tsize> &a, const simple_bitset<Tsize> &b)
{
   simple_bitset<Tsize> res = a;
   res &= b;
   return res;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize>
operator |(const simple_bitset<Tsize> &a, const simple_bitset<Tsize> &b)
{
   simple_bitset<Tsize> res = a;
   res |= b;
   return res;
}

template <size_t Tsize>
constexpr simple_bitset<Tsize>
operator ^(const simple_bitset<Tsize> &a, const simple_bitset<Tsize> &b)
{
   simple_bitset<Tsize> res = a;
   res ^= b;
   return res;
}

} // namespace lcore
} // namespace strmod

#endif
