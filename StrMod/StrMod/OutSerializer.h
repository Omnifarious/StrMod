#ifndef _STR_OutSerializer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.2  1998/06/02 00:30:53  hopper
// Fixed a small bug that caused adding a 2 bytes value to not work
// correctly.
//
// Revision 1.1  1997/04/11 17:44:15  hopper
// Added OutSerializer class for simple serialization of basic data types.
//

#include <LCore/GenTypes.h>
#include <string.h>
#include <string>
#include <bool.h>

#define _STR_OutSerializer_H_

class DataBlockStrChunk;

class OutSerializer {
 public:
   OutSerializer(size_t suggested_size);
   OutSerializer();
   virtual ~OutSerializer();

   inline void AddS1Byte(S1Byte num);
   inline void AddU1Byte(U1Byte num);

   inline void AddS2Byte(S2Byte num);
   inline void AddU2Byte(U2Byte num);

   inline void AddS4Byte(S4Byte num);
   inline void AddU4Byte(U4Byte num);

   void AddBool(bool val)                           { AddU1Byte(U1Byte(val)); }

   void AddString(const string &str);
   void AddString(const char *str);  // C style string.

   inline void AddRaw(const void *data, size_t len);

   inline void SetMinSuggestedSize(size_t size);

   // It is an error to call this function more than once.  The chunk
   // you get has exactly the number of bytes you put in.
   DataBlockStrChunk *TakeChunk();

 private:
   DataBlockStrChunk *cur_chunk_;
   U1Byte *buf_;
   size_t chnklen_;
   size_t cur_pos_;

   void ResizeChunk(size_t newsize);
};

//-----------------------------inline functions--------------------------------

inline void OutSerializer::SetMinSuggestedSize(size_t size)
{
   if (size > chnklen_) {
      ResizeChunk(size);
   }
}

inline void OutSerializer::AddS1Byte(S1Byte num)
{
   SetMinSuggestedSize(cur_pos_ + 1);

   hton(num, buf_ + cur_pos_);
   cur_pos_++;
}

inline void OutSerializer::AddU1Byte(U1Byte num)
{
   SetMinSuggestedSize(cur_pos_ + 1);

   hton(num, buf_ + cur_pos_);
   cur_pos_++;
}

inline void OutSerializer::AddS2Byte(S2Byte num)
{
   SetMinSuggestedSize(cur_pos_ + 2);

   hton(num, buf_ + cur_pos_);
   cur_pos_ += 2;
}

inline void OutSerializer::AddU2Byte(U2Byte num)
{
   SetMinSuggestedSize(cur_pos_ + 2);

   hton(num, buf_ + cur_pos_);
   cur_pos_ += 2;
}

inline void OutSerializer::AddS4Byte(S4Byte num)
{
   SetMinSuggestedSize(cur_pos_ + 4);

   hton(num, buf_ + cur_pos_);
   cur_pos_ += 4;
}

inline void OutSerializer::AddU4Byte(U4Byte num)
{
   SetMinSuggestedSize(cur_pos_ + 4);

   hton(num, buf_ + cur_pos_);
   cur_pos_ += 4;
}

inline void OutSerializer::AddRaw(const void *data, size_t len)
{
   SetMinSuggestedSize(cur_pos_ + len);

   memcpy(buf_ + cur_pos_, data, len);
   cur_pos_ += len;
}

//---

inline OutSerializer &operator <<(OutSerializer &os, S1Byte num)
{
   os.AddS1Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, U1Byte num)
{
   os.AddU1Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, S2Byte num)
{
   os.AddS2Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, U2Byte num)
{
   os.AddU2Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, S4Byte num)
{
   os.AddS4Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, U4Byte num)
{
   os.AddU4Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, const string &str)
{
   os.AddString(str);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, const char *str)
{
   os.AddString(str);
   return(os);
}

#endif
