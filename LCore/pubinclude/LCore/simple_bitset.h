#ifndef _LCORE_simple_bitset_H_  // -*-c++-*-


/* $Header$ */

// For a log, see ..ChangeLog

#include <cstddef>
#include <climits>
#include <string>
#include <array>
#include <limits>

#define _LCORE_simple_bitset_H_

namespace strmod {
namespace lcore {
namespace priv {

typedef ::std::array<unsigned int, 256> bitcounts_t;

constexpr bitcounts_t make_bitcounts()
{
   bitcounts_t bitcounts = {0};
   for (unsigned int i = 1; i < 256; ++i)
   {
      unsigned int shifter = i;
      while (shifter) {
         while (!(shifter & 0x1)) {
            shifter >>= 1;
         }
         bitcounts[i]++;
         shifter >>= 1;
      }
   }
   return bitcounts;
}

//! A non-template base class of common methods that don't need to vary with the template parameter.
class _base_simple_bitset
{
 public:
   typedef unsigned long bits_t;
   static constexpr bits_t allones_ = ::std::numeric_limits<bits_t>::max();
   static constexpr size_t bits_t_bits = sizeof(bits_t) * 8;
   static auto constexpr bits_in = make_bitcounts();
   inline static constexpr size_t countbits(const bits_t bitary[],
                                            size_t size, bits_t lastmask);
   static ::std::string to_string(const bits_t bitary[],
                                  size_t size, bits_t lastmask);
};

inline constexpr size_t _base_simple_bitset::countbits(const bits_t bitary[],
                                                       size_t size,
                                                       bits_t lastmask)
{
   size_t count = 0;
   for (size_t i = 0; i < size; ++i)
   {
      bits_t cur = (i == (size - 1)) ? bitary[i] & lastmask : bitary[i];
      while (cur)
      {
         count += bits_in[cur & 0xff];
         cur >>= 8;
      }
   }
   return count;
}

} // namespace priv

//! A template for a simple statically sized bitset.
template <size_t Tsize> class simple_bitset : private priv::_base_simple_bitset
{
 public:
   typedef simple_bitset<Tsize> self_t;

   inline explicit simple_bitset(bool initial_value = false);
   inline simple_bitset(const simple_bitset<Tsize> &rhs);

   inline self_t &operator &=(const self_t &rhs);
   inline self_t &operator |=(const self_t &rhs);
   inline self_t &operator ^=(const self_t &rhs);
   inline self_t &set();
   inline self_t &set(size_t pos, bool val = true);
   inline self_t &reset();
   inline self_t &reset(size_t pos);
   inline self_t &flip();
   inline self_t &flip(size_t pos);
   inline bool operator [](size_t pos) const               { return test(pos); }
   inline size_t count() const;
   inline size_t size() const                              { return numbits_; }
   inline bool operator ==(const self_t &rhs) const;
   inline bool operator !=(const self_t &rhs) const;
   inline bool test(size_t pos) const;
   inline bool any() const;
   inline bool none() const;
   inline ::std::string to_string() const;

   inline const simple_bitset<Tsize> &operator =(const simple_bitset<Tsize> &rhs);

 private:
   static constexpr size_t numbits_ = Tsize;
   static constexpr size_t bitslen_ = Tsize / bits_t_bits + ((Tsize % bits_t_bits != 0) ? 1 : 0);
   static constexpr bits_t topmask_ = (Tsize % bits_t_bits == 0) ? allones_ : ((1 << Tsize % bits_t_bits) - 1);
   char pad1[12];
   bits_t bits_[bitslen_];
   char pad2[12];
};

//-----------------------------inline functions--------------------------------

template <size_t Tsize>
inline simple_bitset<Tsize>::simple_bitset(bool initial_value)
{
   for (size_t i = 0; i < (bitslen_ - 1); ++i)
   {
      bits_[i] = initial_value ? allones_ : 0;
   }
   bits_[bitslen_ - 1] = initial_value ? topmask_ : 0;
}

template <size_t Tsize>
inline simple_bitset<Tsize>::simple_bitset(const simple_bitset<Tsize> &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] = rhs.bits_[i];
   }
}

template <size_t Tsize> inline const simple_bitset<Tsize> &
simple_bitset<Tsize>::operator =(const simple_bitset<Tsize> &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] = rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::operator &=(const self_t &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] &= rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::operator |=(const self_t &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] |= rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::operator ^=(const self_t &rhs)
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] ^= rhs.bits_[i];
   }
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::set()
{
   for (size_t i = 0; i < (bitslen_ - 1); ++i)
   {
      bits_[i] = allones_;
   }
   bits_[bitslen_ - 1] = topmask_;
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::set(size_t pos, bool val)
{
   const bits_t bval = bits_t(1) << (pos % bits_t_bits);
   const size_t bitpos = pos / bits_t_bits;
   bits_[bitpos] = val ? (bits_[bitpos] | bval) : (bits_[bitpos] & ~bval);
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::reset()
{
   for (size_t i = 0; i < bitslen_; ++i)
   {
      bits_[i] = 0;
   }
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::reset(size_t pos)
{
   return set(pos, false);
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::flip()
{
   for (size_t i = 0; i < (bitslen_ - 1); ++i)
   {
      bits_[i] ^= allones_;
   }
   bits_[bitslen_ - 1] ^= topmask_;
   return *this;
}

template <size_t Tsize> inline simple_bitset<Tsize> &
simple_bitset<Tsize>::flip(size_t pos)
{
   const bits_t bval = bits_t(1) << (pos % bits_t_bits);
   const size_t bitpos = bitslen_ - (pos / bits_t_bits);
   bits_[bitpos] ^= bval;
   return *this;
}

template <size_t Tsize> inline size_t
simple_bitset<Tsize>::count() const
{
   return countbits(bits_, bitslen_, topmask_);
}

template <size_t Tsize> inline bool
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

template <size_t Tsize> inline bool
simple_bitset<Tsize>::operator !=(const self_t &rhs) const
{
   return !(*this == rhs);
}

template <size_t Tsize> inline bool
simple_bitset<Tsize>::test(size_t pos) const
{
   const bits_t bval = bits_t(1) << (pos % bits_t_bits);
   const size_t bitpos = pos / bits_t_bits;
   return (bits_[bitpos] & bval) != 0;
}

template <size_t Tsize> inline bool
simple_bitset<Tsize>::any() const
{
   return !none();
}

template <size_t Tsize> inline bool
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

template <size_t Tsize> inline ::std::string
simple_bitset<Tsize>::to_string() const
{
   return _base_simple_bitset::to_string(bits_, bitslen_, topmask_);
}

#include <LCore/simple_bitset_optim.h>

template <size_t Tsize> inline const simple_bitset<Tsize>
operator &(const simple_bitset<Tsize> &a, const simple_bitset<Tsize> &b)
{
   simple_bitset<Tsize> res = a;
   res &= b;
   return res;
}

template <size_t Tsize> inline const simple_bitset<Tsize>
operator |(const simple_bitset<Tsize> &a, const simple_bitset<Tsize> &b)
{
   simple_bitset<Tsize> res = a;
   res |= b;
   return res;
}

template <size_t Tsize> inline const simple_bitset<Tsize>
operator ^(const simple_bitset<Tsize> &a, const simple_bitset<Tsize> &b)
{
   simple_bitset<Tsize> res = a;
   res ^= b;
   return res;
}

} // namespace lcore
} // namespace strmod

#endif
