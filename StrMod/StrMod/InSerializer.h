#ifndef _STR_InSerializer_H_  // -*-c++-*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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
#include <string.h>
#include <string>

#define _STR_InSerializer_H_

class StrChunkPtr;

class InSerializer {
 public:
   InSerializer(const StrChunkPtr &ptr);
   InSerializer(const void *buf, size_t len);
   virtual ~InSerializer();

   S1Byte GetS1Byte();
   U1Byte GetU1Byte();

   S2Byte GetS2Byte();
   U2Byte GetU2Byte();

   S4Byte GetS4Byte();
   U4Byte GetU4Byte();

   inline bool GetBool()                               { return(GetU1Byte()); }

   const string GetString();

   void GetRaw(void *destbuf, size_t len);

   size_t BytesLeft();
   // Someday, I'll be able to use exceptions instead.
   bool HadError() const                               { return(had_error_); }

 private:
   struct Impl;

   Impl &impl_;
   bool had_error_;
};

//-----------------------------inline functions--------------------------------

inline InSerializer &operator >>(InSerializer &is, S1Byte &num)
{
   num = is.GetS1Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, U1Byte &num)
{
   num = is.GetU1Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, S2Byte &num)
{
   num = is.GetS2Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, U2Byte &num)
{
   num = is.GetU2Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, S4Byte &num)
{
   num = is.GetS4Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, U4Byte &num)
{
   num = is.GetU4Byte();
   return(is);
}

inline InSerializer &operator >>(InSerializer &is, string &str)
{
   str = is.GetString();
   return(is);
}

#endif
