#ifndef _LCORE_GenTypes_H_

/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For a log, see ../ChangeLog
//
// Revision 1.4  1998/02/08 04:40:35  hopper
// Untested efficiency improvements.
//
// Revision 1.3  1996/07/08 02:26:47  hopper
// Added include of byteorder file on 386 architectures.
// Added const in a lot of places that needed it.
//
// Revision 1.2  1996/03/26 16:22:59  hopper
// Moved ReferenceCounting class over from Container library.
// Fixed up class numbering system so LCore uses a seperate library number
// from the container library.
//
// Revision 1.1.1.1  1995/07/22 04:09:24  hopper
// Imported sources
//
// Revision 0.3  1994/10/30  04:40:26  hopper
// Moved various things into the new LCore library.
//
// Revision 0.2  1994/10/05  06:00:57  hopper
// Some changes to make it work better with gcc.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#include <arpa/inet.h>

#define _LCORE_GenTypes_H_

namespace strmod {
namespace lcore {

#ifdef BYTE_ORDER_DEFINED
#   if !(defined(ORDER_UNIX) || defined(ORDER_XINU))
#      error Byte order defined macro is defined, but no valid byte orderings \
	     are defined.
#   endif
#else
#   if defined(ORDER_UNIX) || defined(ORDER_XINU)
#      error "Byte order defined macro isn't defined, but a valid order is."
#   endif
#endif

#if defined(__MSDOS__) || defined(MSDOS)
   typedef unsigned long  U4Byte;
   typedef long           S4Byte;
   typedef unsigned int   U2Byte;
   typedef int            S2Byte;
   typedef unsigned char  U1Byte;
   typedef signed char    S1Byte;
   typedef float          IEEE_Float32;
   typedef double         IEEE_Float64;
   #if !defined(BYTE_ORDER_DEFINED)
      #define ORDER_XINU   // All 8x86 or 80x86 MSDOS machines are ORDER_XINU
      #define BYTE_ORDER_DEFINED
   #endif
#else // UNIX is default   defined(UNIX)
#ifdef __alpha
   typedef unsigned int   U4Byte;
#else
   typedef unsigned long  U4Byte;
#endif
#ifdef __alpha
   typedef int            S4Byte;
#else
   typedef long           S4Byte;
#endif
   typedef unsigned short U2Byte;
   typedef signed short   S2Byte;
   typedef unsigned char  U1Byte;
   typedef signed char    S1Byte;
   typedef float          IEEE_Float32;
   typedef double         IEEE_Float64;
   #if !defined(BYTE_ORDER_DEFINED)
      #define ORDER_UNIX
      #define BYTE_ORDER_DEFINED
   #endif
#endif


#ifdef __cplusplus

// These guys are out here for consistency. You never really have to mix
// around a 1 byte value. (I hope)

#ifndef BYTE_ORDER_DEFINED
#   error No one has defined a byte ordering.
#endif

   inline void hton(const U1Byte n, U1Byte *c)
   {
      c[0] = n;
   }
   inline void ntoh(const U1Byte *c, U1Byte &n)
   {
      n = c[0];
   }

   inline void hton(const S1Byte n, U1Byte *c)
   {
      *((S1Byte *)(c)) = n;
   }
   inline void ntoh(const U1Byte *c, S1Byte &n)
   {
      n = *((const S1Byte *)(c));
   }

   #ifdef ORDER_UNIX
      inline void hton(const U4Byte n, U1Byte *c)
      {
	 *((U4Byte *)(c)) = n;
      }
      inline void ntoh(const U1Byte *c, U4Byte &n)
      {
	 n = *((const U4Byte *)(c));
      }

      inline void hton(const S4Byte n, U1Byte *c)
      {
	 *((S4Byte *)(c)) = n;
      }
      inline void ntoh(const U1Byte *c, S4Byte &n)
      {
	 n = *((const S4Byte *)(c));
      }


      inline void hton(const U2Byte n, U1Byte *c)
      {
	 *((U2Byte *)(c)) = n;
      }
      inline void ntoh(const U1Byte *c, U2Byte &n)
      {
	 n = *((const U2Byte *)(c));
      }

      inline void hton(const S2Byte n, U1Byte *c)
      {
	 *((S2Byte *)(c)) = n;
      }
      inline void ntoh(const U1Byte *c, S2Byte &n)
      {
	 n = *((const S2Byte *)(c));
      }


      inline void hton(const IEEE_Float32 n, U1Byte *c)
      {
	 *((IEEE_Float32 *)(c)) = n;
      }
      inline void hton(const U1Byte *c, IEEE_Float32 &n)
      {
	 n = *((const IEEE_Float32 *)(c));
      }

      inline void hton(const IEEE_Float64 n, U1Byte *c)
      {
	 *((IEEE_Float64 *)(c)) = n;
      }
      inline void hton(const U1Byte *c, IEEE_Float64 &n)
      {
	 n = *((const IEEE_Float64 *)(c));
      }

   #elif defined(ORDER_XINU)

#   ifdef __GNUG__
      inline void hton(const U4Byte n, U1Byte *c)
      {
	 *((U4Byte *)(c)) = htonl(n);
      }
      inline void ntoh(const U1Byte *c, U4Byte &n)
      {
	 n = ntohl(*((const U4Byte *)(c)));
      }
      inline void hton(const S4Byte n, U1Byte *c)
      {
	 *((S4Byte *)(c)) = htonl(n);
      }
      inline void ntoh(const U1Byte *c, S4Byte &n)
      {
	 n = ntohl(*((const S4Byte *)(c)));
      }

      inline void hton(const U2Byte n, U1Byte *c)
      {
	 *((U2Byte *)(c)) = htons(n);
      }
      inline void ntoh(const U1Byte *c, U2Byte &n)
      {
	 n = ntohs(*((const U2Byte *)(c)));
      }
      inline void hton(const S2Byte n, U1Byte *c)
      {
	 *((S2Byte *)(c)) = htons(n);
      }
      inline void ntoh(const U1Byte *c, S2Byte &n)
      {
	 n = ntohs(*((const S2Byte *)(c)));
      }
#else
      inline void hton(const U4Byte n, U1Byte *c)
      {
	 *c++ = U1Byte((n >> 24) & 0xffU);
	 *c++ = U1Byte((n >> 16) & 0xffU);
	 *c++ = U1Byte((n >> 8) & 0xffU);
	 *c = U1Byte(n & 0xffU);
      }
      inline void ntoh(const U1Byte *c, U4Byte &n)
      {
	 n = (U4Byte(c[0] & 0xffU) << 24) | (U4Byte(c[1] & 0xffU) << 16) |
	     (U4Byte(c[2] & 0xffU) << 8)  |  U4Byte(c[3] & 0xffU);
      }

      inline void hton(const S4Byte n, U1Byte *c)
      {
	 hton(U4Byte(n), c);  // Essentially the same operation...
      }
      inline void ntoh(const U1Byte *c, S4Byte &n)  {
	 n = (U4Byte(c[0] & 0xffU) << 24) | (U4Byte(c[1] & 0xffU) << 16) |
	     (U4Byte(c[2] & 0xffU) << 8)  |  U4Byte(c[3] & 0xffU);
      }


      inline void hton(const U2Byte n, U1Byte *c)
      {
	 *c++ = U1Byte((n >> 8) & 0xffU);
	 *c = (n & 0xffU);
      }
      inline void ntoh(const U1Byte *c, U2Byte &n)  {
	 n = (U2Byte(c[0] & 0xffU) << 8) | U2Byte(c[1] & 0xffU);
      }

      inline void hton(const S2Byte n, U1Byte *c)
      {
	 hton(U2Byte(n), c);
      }
      inline void ntoh(const U1Byte *c, S2Byte &n)
      {
	 n = S2Byte((U2Byte(c[0] & 0xffU) << 8) | U2Byte(c[1] & 0xffU));
      }
#endif


      inline void hton(const IEEE_Float32 &n, U1Byte *c)
      {
	 const U1Byte * const from = ((const U1Byte *)(&n));

	 c[3] = from[0];
	 c[2] = from[1];
	 c[1] = from[2];
	 c[0] = from[3];
      }
      inline void ntoh(const U1Byte *c, IEEE_Float32 &n)
      {
	 U1Byte * const to = ((U1Byte *)(&n));

	 to[0] = c[3];
	 to[1] = c[2];
	 to[2] = c[1];
	 to[3] = c[0];
      }

      inline void hton(const IEEE_Float64 &n, U1Byte *c)
      {
	 const U1Byte * const from = ((const U1Byte *)(&n));

	 c[7] = from[0];
	 c[6] = from[1];
	 c[5] = from[2];
	 c[4] = from[3];
	 c[3] = from[4];
	 c[2] = from[5];
	 c[1] = from[6];
	 c[0] = from[7];
      }
      inline void ntoh(const U1Byte *c, IEEE_Float64 &n)
      {
	 U1Byte * const to = ((U1Byte *)(&n));

	 to[0] = c[7];
	 to[1] = c[6];
	 to[2] = c[5];
	 to[3] = c[4];
	 to[4] = c[3];
	 to[5] = c[2];
	 to[6] = c[1];
	 to[7] = c[0];
      }
   #endif
#endif

} // namespace lcore
} // namespace strmod

#endif
