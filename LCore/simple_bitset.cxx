/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "simple_bitset.h"
#endif

#ifdef __GNUG__
#  pragma implementation "simple_bitset_optim.h"
#endif

#include "LCore/simple_bitset.h"
#include <algorithm>

namespace priv {

const unsigned int _base_simple_bitset::bits_in[256] = {
   0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
   1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
   1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
   2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
   1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
   2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
   2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
   3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
   1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
   2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
   2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
   3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
   2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
   3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
   3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
   4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

::std::string _base_simple_bitset::to_string(const bits_t bitary[],
                                             size_t size, bits_t lastmask)
{
   const size_t mysize = size;
   ::std::string str = "b";
   if (size == 0)
      return "";
   for (size_t i = 0; i < mysize; ++i)
   {
      const bits_t curnum = bitary[i];
      const bits_t maxmask = (i == (mysize - 1)) ? lastmask : allones_;
      bits_t curmask = 1U;
      while (curmask && (curmask < maxmask))
      {
         if (curnum & curmask)
         {
            str += '1';
         }
         else
         {
            str += '0';
         }
         curmask <<= 1;
      }
   }
   ::std::reverse(str.begin(), str.end());
   return str;
}
}
