#ifndef _STR_InSerializer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
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
