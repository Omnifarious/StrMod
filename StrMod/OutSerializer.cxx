/* $Header$ */

// $Log$
// Revision 1.2  1999/01/12 04:13:40  hopper
// All kinds of changes to make the code more portable and compile
// properly with the DEC compiler.
//
// Revision 1.1  1997/04/11 17:44:04  hopper
// Added OutSerializer class for simple serialization of basic data types.
//

#ifdef __GNUG__
#  pragma implementation "OutSerializer.h"
#endif

#include <StrMod/OutSerializer.h>
#include <StrMod/DBStrChunk.h>

OutSerializer::OutSerializer(size_t suggested_size) : cur_pos_(0)
{
   cur_chunk_ = new DataBlockStrChunk(suggested_size);
   chnklen_ = cur_chunk_->Length();
   buf_ = cur_chunk_->GetCharP();
}

OutSerializer::OutSerializer() : cur_pos_(0)
{
   cur_chunk_ = new DataBlockStrChunk(256);
   chnklen_ = cur_chunk_->Length();
   buf_ = cur_chunk_->GetCharP();
}

OutSerializer::~OutSerializer()
{
   if (cur_chunk_) {
      delete cur_chunk_;
   }
}

static void AddCharStr(OutSerializer &out, const char *data, size_t len)
{
   if (len >= 65535) {
      len = 65534;
   }

   U2Byte netlen = len;

   out.AddU2Byte(netlen + 1);  // Add 1 for trailing '\0';
   out.AddRaw(data, netlen);
   out.AddU1Byte('\0');  // Add trailing '\0';
}

void OutSerializer::AddString(const string &str)
{
   size_t len = str.length();

   AddCharStr(*this, str.c_str(), len);
}

void OutSerializer::AddString(const char *str)
{
   AddCharStr(*this, str, strlen(str));
}

DataBlockStrChunk *OutSerializer::TakeChunk()
{
   assert(cur_chunk_ != 0);

   DataBlockStrChunk *temp = cur_chunk_;

   cur_chunk_ = 0;
   buf_ = 0;
   chnklen_ = 0;
   temp->Resize(cur_pos_);
   cur_pos_ = 0;
   return(temp);
}

void OutSerializer::ResizeChunk(size_t newsize)
{
   assert(cur_chunk_ != 0);

   cur_chunk_->Resize(newsize + newsize / 16);
   buf_ = cur_chunk_->GetCharP();
   chnklen_ = cur_chunk_->Length();
   assert(chnklen_ >= newsize);
}
