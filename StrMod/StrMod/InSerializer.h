#ifndef _STR_InSerializer_H_  // -*-c++-*-

/*
 * Copyright 1991-2002 Eric M. Hopper <hopper@omnifarious.org>
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
//
// Revision 1.2  1999/10/21 02:09:10  hopper
// Removed all references to <bool.h> and changed all references to
// bool_val and bool_cst to bool.
//
// Revision 1.1  1997/04/14 23:55:14  hopper
// Added class to aid in serialization of simple data tyes.
//

#include <LCore/GenTypes.h>
#include <string>
#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif

#define _STR_InSerializer_H_

namespace strmod {
namespace strmod {

/** \class InSerializer InSerializer.h StrMod/InSerializer.h
 * Provides a simple way to 'deserialize' a StrChunk.
 *
 * This converts from a stream of bytes into more structured data.
 *
 * See class OutSerializer for a more detailed explanation.
 */
class InSerializer
{
 public:
   //! Construct an InSerializer that reads bytes from \c ptr.
   // \param ptr A pointer to a StrChunk to convert from.
   InSerializer(const StrChunkPtr &ptr);
   //! Construct an InSerializer that reads bytes directly from a memory area.
   // \param buf The beginning of the memory region to convert from.
   // \param len The number of bytes in the memory region to convert from.
   InSerializer(const void *buf, size_t len);
   //! It destroys things.  :-)
   virtual ~InSerializer();

   //! Get a signed 1 octet value (2's complement) and move forward 1 octet.
   lcore::S1Byte GetS1Byte();
   //! Get an unsigned 1 octet value and move forward 1 octet.
   lcore::U1Byte GetU1Byte();

   //! Get a signed 2 octet value (2's complement) and move forward 2 octets.
   lcore::S2Byte GetS2Byte();
   //! Get an unsigned 2 octet value and move forward 2 octets.
   lcore::U2Byte GetU2Byte();

   //! Get a signed 4 octet value (2's complement) and move forward 2 octets.
   lcore::S4Byte GetS4Byte();
   //! Get an unsigned 4 octet value and move forward 2 octets.
   lcore::U4Byte GetU4Byte();

   //! Get a bool value and move forward 1 octet.
   inline bool GetBool()                               { return(GetU1Byte()); }

   //! Get a string value.  See OutSerializer for more on format.
   const ::std::string GetString();

   //! Get a \c len bytes and dump them in \c destbuf.
   // \param destbuf A memory area to copy bytes into.
   // \param len The number of bytes to copy.
   void GetRaw(void *destbuf, size_t len);

   //! How many bytes are there left to read?
   size_t BytesLeft();
   // Someday, I'll be able to use exceptions instead.
   //! Is the serializer in an error state.
   bool HadError() const                               { return(had_error_); }

 private:
   struct Impl;

   Impl &impl_;
   bool had_error_;
};

//-----------------------------inline functions--------------------------------

inline InSerializer &operator >>(InSerializer &is, lcore::S1Byte &num)
{
   num = is.GetS1Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, lcore::U1Byte &num)
{
   num = is.GetU1Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, lcore::S2Byte &num)
{
   num = is.GetS2Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, lcore::U2Byte &num)
{
   num = is.GetU2Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, lcore::S4Byte &num)
{
   num = is.GetS4Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, lcore::U4Byte &num)
{
   num = is.GetU4Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, ::std::string &str)
{
   str = is.GetString();
   return(is);
}

};  // namespace strmod
};  // namespace strmod

#endif
