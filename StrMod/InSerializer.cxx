/* $Header$ */

// $Log$
// Revision 1.1  1997/04/14 23:55:06  hopper
// Added class to aid in serialization of simple data tyes.
//

#ifdef __GNUG__
#  pragma implementation "InSerializer.h"
#endif

#include <StrMod/InSerializer.h>
#include <StrMod/StrChunk.h>
#include <StrMod/StrChunkPtr.h>
#include <StrMod/GroupVector.h>
#include <StrMod/GV_Iterator.h>

static GroupVector dummy_gv(1);

struct InSerializer::Impl {
   inline Impl(const void *buf, size_t size);
   inline Impl(const StrChunkPtr &ptr);
   inline ~Impl();
   inline size_t BytesLeft() const;

   GroupVector vec;
   GroupVector::Iterator i;
   size_t bytesread_;
   StrChunkPtr chnkptr_;
};

inline InSerializer::Impl::Impl(const void *buf, size_t size)
     : vec(1), bytesread_(0), i(dummy_gv)
{
   vec.SetVec(0, buf, size);
   i = vec.begin();
}

inline InSerializer::Impl::Impl(const StrChunkPtr &ptr)
     : vec(ptr->NumSubGroups()), i(dummy_gv), bytesread_(0), chnkptr_(ptr)
{
   chnkptr_->SimpleFillGroupVec(vec);
   i = vec.begin();
}

inline InSerializer::Impl::~Impl()
{
   i = dummy_gv.begin();
}

inline size_t InSerializer::Impl::BytesLeft() const
{
   return(vec.TotalLength() - bytesread_);
}

InSerializer::InSerializer(const StrChunkPtr &ptr)
     : impl_(*(new Impl(ptr))), had_error_(false)
{
}

InSerializer::InSerializer(const void *buf, size_t len)
     : impl_(*(new Impl(buf, len))), had_error_(false)
{
}

InSerializer::~InSerializer()
{
   delete &impl_;
}

S1Byte InSerializer::GetS1Byte()
{
   if (HadError()) {
      return(0);
   } else if (impl_.BytesLeft() < 1) {
      had_error_ = true;
      return(0);
   } else {
      U1Byte ch = *(impl_.i);
      ++impl_.i;
      ++impl_.bytesread_;

      S1Byte val;

      ntoh(&ch, val);
      return(val);
   }
}

U1Byte InSerializer::GetU1Byte()
{
   if (HadError()) {
      return(0);
   } else if (impl_.BytesLeft() < 1) {
      had_error_ = true;
      return(0);
   } else {
      U1Byte ch = *(impl_.i);
      ++impl_.i;
      ++impl_.bytesread_;

      U1Byte val;

      ntoh(&ch, val);
      return(val);
   }
}

S2Byte InSerializer::GetS2Byte()
{
   if (HadError()) {
      return(0);
   } else if (impl_.BytesLeft() < 2) {
      had_error_ = true;
      return(0);
   } else {
      U1Byte ch[2];
      S2Byte val;

      ch[0] = *(impl_.i);
      ++impl_.i;
      ch[1] = *(impl_.i);
      ++impl_.i;
      impl_.bytesread_ += 2;
      ntoh(ch, val);
      return(val);
   }
}

U2Byte InSerializer::GetU2Byte()
{
   if (HadError()) {
      return(0);
   } else if (impl_.BytesLeft() < 2) {
      had_error_ = true;
      return(0);
   } else {
      U1Byte ch[2];
      U2Byte val;

      ch[0] = *(impl_.i);
      ++impl_.i;
      ch[1] = *(impl_.i);
      ++impl_.i;
      impl_.bytesread_ += 2;
      ntoh(ch, val);
      return(val);
   }
}

S4Byte InSerializer::GetS4Byte()
{
   if (HadError()) {
      return(0);
   } else if (impl_.BytesLeft() < 4) {
      had_error_ = true;
      return(0);
   } else {
      U1Byte ch[4];
      S4Byte val;

      ch[0] = *(impl_.i);
      ++impl_.i;
      ch[1] = *(impl_.i);
      ++impl_.i;
      ch[2] = *(impl_.i);
      ++impl_.i;
      ch[3] = *(impl_.i);
      ++impl_.i;
      impl_.bytesread_ += 4;
      ntoh(ch, val);
      return(val);
   }
}

U4Byte InSerializer::GetU4Byte()
{
   if (HadError()) {
      return(0);
   } else if (impl_.BytesLeft() < 4) {
      had_error_ = true;
      return(0);
   } else {
      U1Byte ch[4];
      U4Byte val;

      ch[0] = *(impl_.i);
      ++impl_.i;
      ch[1] = *(impl_.i);
      ++impl_.i;
      ch[2] = *(impl_.i);
      ++impl_.i;
      ch[3] = *(impl_.i);
      ++impl_.i;
      impl_.bytesread_ += 4;
      ntoh(ch, val);
      return(val);
   }
}

const string InSerializer::GetString()
{
   if (HadError()) {
      return("");
   } else if (impl_.BytesLeft() < 3) {
      had_error_ = true;
      return("");
   } else {
      U1Byte ch[2];
      U2Byte len;

      ch[0] = *(impl_.i);
      ++impl_.i;
      ch[1] = *(impl_.i);
      ++impl_.i;

      ntoh(ch, len);
      if ((len < 1) || (len > impl_.BytesLeft())) {
	 had_error_ = true;
	 return("");
      } else {
	 string result;
	 U2Byte templen = len - 1;  // Skip trailing '\0'

	 result.reserve(templen);  // Weak attempt to optimize performance.
	 while (templen--) {
	    result += *(impl_.i);
	    ++impl_.i;
	 }
	 ++impl_.i;  // Move iterator past trailing '\0'
	 impl_.bytesread_ += (len + 2);  // + 2 for the initial length bytes.
	 return(result);
      }
   }
}

void InSerializer::GetRaw(void *destbuf, size_t len)
{
   if (!HadError()) {
      if (impl_.BytesLeft() >= len) {
	 U1Byte *dest = static_cast<U1Byte *>(destbuf);
	 size_t templen = len;

	 while (templen--) {
	    *dest++ = *(impl_.i);
	    ++impl_.i;
	 }
	 impl_.bytesread_ += len;
      } else {
	 had_error_ = true;
      }
   }
}

size_t InSerializer::BytesLeft()
{
   return(impl_.BytesLeft());
}