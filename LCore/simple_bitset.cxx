/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "simple_bitset.h"
#endif

#include "LCore/simple_bitset.h"
#include <algorithm>

namespace strmod {
namespace lcore {
namespace priv {

::std::string base_simple_bitset::to_string(const bits_t bitary[],
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
} // namespace priv
} // namespace lcore
} // namespace strmod
