/* $Header$ */

// For a log, see ./ChangeLog
//
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

#include "StrMod/OutSerializer.h"
#include "StrMod/BufferChunkFactory.h"
#include "StrMod/DynamicBuffer.h"

inline OutSerializer::State::State(BufferChunk::Factory *fact)
     : fact_(fact), cur_chunk_(0), buf_(0), chnklen_(0), cur_pos_(0)
{
}

inline OutSerializer::State::State(const State &b)
     : fact_(b.fact_), cur_chunk_(b.cur_chunk_),
       buf_(b.buf_), chnklen_(b.chnklen_), cur_pos_(b.cur_pos_)
{
}

inline const OutSerializer::State &
OutSerializer::State::operator =(const State &b)
{
   cur_chunk_ = b.cur_chunk_;
   buf_ = b.buf_;
   chnklen_ = b.chnklen_;
   cur_pos_ = b.cur_pos_;
   return(*this);
}

OutSerializer::OutSerializer(size_t suggested_size)
	  : state_(0), external_state_(0)
{
   state_.cur_chunk_ = new DynamicBuffer;
   state_.cur_chunk_->resize(suggested_size);
   state_.chnklen_ = state_.cur_chunk_->Length();
   state_.buf_ = state_.cur_chunk_->getCharP();
}

OutSerializer::OutSerializer(State &savedstate)
     : state_(savedstate), external_state_(&savedstate)
{
   if (state_.cur_chunk_ == 0)
   {
      assert(state_.fact_ != 0);
      state_.cur_chunk_ = state_.fact_->makeChunk();
      state_.chnklen_ = state_.cur_chunk_->Length();
      state_.cur_pos_ = 0;
   }
   else
   {
      assert(state_.chnklen_ == state_.cur_chunk_->Length());
      assert(state_.cur_pos_ <= state_.chnklen_);
   }
}

OutSerializer::OutSerializer() : state_(0), external_state_(0)
{
   state_.cur_chunk_ = new DynamicBuffer;
   state_.cur_chunk_->resize(256);
   state_.cur_pos_ = 0;
   state_.chnklen_ = state_.cur_chunk_->Length();
   state_.buf_ = state_.cur_chunk_->getCharP();
}

OutSerializer::~OutSerializer()
{
   if (external_state_)
   {
      *external_state_ = state_;
   }
   else if (state_.cur_chunk_)
   {
      delete state_.cur_chunk_;
      state_.cur_chunk_ = 0;
   }
}

static void SAddCharStr(OutSerializer &out, const char *data, size_t len)
{
   if (len >= 65535)
   {
      len = 65534;
   }

   U2Byte netlen = len;

   out.addU2Byte(netlen + 1);  // Add 1 for trailing '\0';
   out.addRaw(data, netlen);
   out.addU1Byte('\0');  // Add trailing '\0';
}

void OutSerializer::addString(const string &str)
{
   size_t len = str.length();

   SAddCharStr(*this, str.c_str(), len);
}

void OutSerializer::addString(const char *str)
{
   SAddCharStr(*this, str, strlen(str));
}

BufferChunk *OutSerializer::takeChunk()
{
   assert(state_.cur_chunk_ != 0);

   BufferChunk *temp = state_.cur_chunk_;

   state_.cur_chunk_ = 0;
   state_.buf_ = 0;
   state_.chnklen_ = 0;
   temp->resize(state_.cur_pos_);
   state_.cur_pos_ = 0;
   return(temp);
}

void OutSerializer::resizeChunk(size_t newsize)
{
   assert(state_.cur_chunk_ != 0);

   if ((newsize - state_.chnklen_) < 256)
   {
      state_.cur_chunk_->resize(newsize + newsize / 16);
   }
   else
   {
      state_.cur_chunk_->resize(newsize + newsize / 256);
   }
   state_.buf_ = state_.cur_chunk_->getCharP();
   state_.chnklen_ = state_.cur_chunk_->Length();
   assert(state_.chnklen_ >= newsize);
}
