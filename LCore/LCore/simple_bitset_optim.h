#ifndef _LCORE_simple_bitset_optim_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#define _LCORE_simple_bitset_optim_H_

template <size_t Tsize> class _single_int_bitset : private priv::_base_simple_bitset
{
 public:
   typedef _single_int_bitset<Tsize> self_t;

   inline explicit _single_int_bitset(bool initial_value = false);
   inline _single_int_bitset(const _single_int_bitset<Tsize> &rhs);

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

 private:
   static const size_t numbits_ = Tsize;
   static const bits_t topmask_ = (Tsize == bits_t_bits) ? allones_ : ((1UL << Tsize) - 1);
   bits_t bits_;
};

//-----------------------------inline functions--------------------------------

template <size_t Tsize>
inline _single_int_bitset<Tsize>::_single_int_bitset(bool initial_value)
{
   bits_ = initial_value ? topmask_ : 0;
}

template <size_t Tsize>
inline _single_int_bitset<Tsize>::_single_int_bitset(const _single_int_bitset<Tsize> &rhs)
{
   bits_ = rhs.bits_;
}
template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::operator &=(const self_t &rhs)
{
   bits_ &= rhs.bits_;
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::operator |=(const self_t &rhs)
{
   bits_ |= rhs.bits_;
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::operator ^=(const self_t &rhs)
{
   bits_ ^= rhs.bits_;
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::set()
{
   bits_ = topmask_;
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::set(size_t pos, bool val)
{
   bits_t bval = bits_t(1) << pos;
   bits_ = val ? (bits_ | (bval & topmask_)) : (bits_ & ~bval);
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::reset()
{
   bits_ = 0;
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::reset(size_t pos)
{
   return set(pos, false);
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::flip()
{
   bits_ ^= topmask_;
   return *this;
}

template <size_t Tsize> inline _single_int_bitset<Tsize> &
_single_int_bitset<Tsize>::flip(size_t pos)
{
   bits_t bval = (bits_t(1) << pos) & topmask_;
   bits_ ^= bval;
   return *this;
}

template <size_t Tsize> inline size_t
_single_int_bitset<Tsize>::count() const
{
   return countbits(&bits_, 1, topmask_);
}

template <size_t Tsize> inline bool
_single_int_bitset<Tsize>::operator ==(const self_t &rhs) const
{
   return bits_ == rhs.bits_;
}

template <size_t Tsize> inline bool
_single_int_bitset<Tsize>::operator !=(const self_t &rhs) const
{
   return !(*this == rhs);
}

template <size_t Tsize> inline bool
_single_int_bitset<Tsize>::test(size_t pos) const
{
   bits_t bval = bits_t(1) << pos;
   return (bits_ & bval) != 0;
}

template <size_t Tsize> inline bool
_single_int_bitset<Tsize>::any() const
{
   return !none();
}

template <size_t Tsize> inline bool
_single_int_bitset<Tsize>::none() const
{
   return bits_ == 0;
}

template <size_t Tsize> inline ::std::string
_single_int_bitset<Tsize>::to_string() const
{
   return _base_simple_bitset::to_string(&bits_, 1, topmask_);
}

//--

//! Specialized to make the compiler optimize better.
template <> class simple_bitset<1> : public _single_int_bitset<1>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<1>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<2> : public _single_int_bitset<2>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<2>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<3> : public _single_int_bitset<3>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<3>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<4> : public _single_int_bitset<4>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<4>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<5> : public _single_int_bitset<5>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<5>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<6> : public _single_int_bitset<6>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<6>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<7> : public _single_int_bitset<7>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<7>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<8> : public _single_int_bitset<8>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<8>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<9> : public _single_int_bitset<9>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<9>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<10> : public _single_int_bitset<10>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<10>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<11> : public _single_int_bitset<11>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<11>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<12> : public _single_int_bitset<12>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<12>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<13> : public _single_int_bitset<13>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<13>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<14> : public _single_int_bitset<14>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<14>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<15> : public _single_int_bitset<15>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<15>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<16> : public _single_int_bitset<16>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<16>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<17> : public _single_int_bitset<17>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<17>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<18> : public _single_int_bitset<18>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<18>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<19> : public _single_int_bitset<19>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<19>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<20> : public _single_int_bitset<20>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<20>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<21> : public _single_int_bitset<21>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<21>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<22> : public _single_int_bitset<22>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<22>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<23> : public _single_int_bitset<23>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<23>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<24> : public _single_int_bitset<24>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<24>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<25> : public _single_int_bitset<25>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<25>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<26> : public _single_int_bitset<26>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<26>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<27> : public _single_int_bitset<27>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<27>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<28> : public _single_int_bitset<28>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<28>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<29> : public _single_int_bitset<29>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<29>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<30> : public _single_int_bitset<30>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<30>(initial_value)
      {
      }
};
//! Specialized to make the compiler optimize better.
template <> class simple_bitset<31> : public _single_int_bitset<31>
{
  public:
   explicit simple_bitset(bool initial_value = false)
      : _single_int_bitset<31>(initial_value)
      {
      }
};

#endif
