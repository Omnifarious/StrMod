#ifndef _STR_DBStrChunk_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.4  1999/01/12 04:17:44  hopper
// All kinds of changes to make the code more portable and compile
// properly with the DEC compiler.
//
// Revision 1.3  1999/01/08 07:21:33  hopper
// Added better comments.
//
// Revision 1.2  1996/09/02 23:14:03  hopper
// Changed inline definition of constructor to take correct type as
// argument.
//
// Revision 1.1  1996/06/29 06:48:54  hopper
// Moved from StrChunk.h and StrChunk.cc along with re-working for new
// StrChunk handling.
//

#include <string.h>  // memcpy
#include <sys/types.h>
#include <StrMod/StrChunk.h>
#include <StrMod/LinearExtent.h>

#define _STR_DBStrChunk_H_

//--------------------------class DataBlockStrChunk----------------------------

//: A reference counted chunk of raw bytes.
class DataBlockStrChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   //: Construct an empty chunk
   DataBlockStrChunk() : buf_(0), buflen_(0)           { }
   //: Construct a chunk of size length.
   inline DataBlockStrChunk(size_t length);
   //: Construct a chunk containing a copy of length bytes of memory
   //: starting at address mem.
   inline DataBlockStrChunk(const void *mem, size_t length);
   virtual ~DataBlockStrChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: See the base class StrChunk.
   inline virtual unsigned int Length() const;
   //: See the base class StrChunk.  Returns 1 if Length() is > 0.
   inline virtual unsigned int NumSubGroups() const;
   //: See the base class StrChunk.  Returns 1 or 0, if extent contains more
   //: than 0 bytes.
   inline virtual unsigned int NumSubGroups(const LinearExtent &extent) const;
   //: See the base class StrChunk.
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   //: See the base class StrChunk.
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   //: Returns the byte at index bnum.
   // If bnum is out of range, the behavior is undefined.
   virtual unsigned char &operator [](unsigned int bnum) const;
   //: Gets a void pointer to the data.
   inline void *GetVoidP() const;
   //: Gets a 'byte' pointer to the data.
   inline unsigned char *GetCharP() const;

   //: Change the size of the chunk to newsize.
   // If the allocation fails, the chunk remains the size it was before the
   // allocation was attempted.  Theoretically, this should only happen if you
   // enlarge the chunk, but it depends on your system's realloc
   // implementation.
   void Resize(size_t newsize);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   //: See the base class StrChunk.
   virtual void i_DropUnused(const LinearExtent &usedextent, KeepDir keepdir);

 private:
   void *buf_;
   size_t buflen_;

   void operator =(const DataBlockStrChunk &b);
   DataBlockStrChunk(const DataBlockStrChunk &b);
};

//--------------------inline functions for DataBlockStrChunk-------------------

inline DataBlockStrChunk::DataBlockStrChunk(size_t length)
     : buf_(0), buflen_(0)
{
   Resize(length);
}

inline DataBlockStrChunk::DataBlockStrChunk(const void *mem,
					    size_t length)
     : buf_(0), buflen_(0)
{
   if (buflen_ > 0) {
      Resize(length);
      memcpy(buf_, mem, buflen_);
   }
}

inline int DataBlockStrChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline unsigned int DataBlockStrChunk::Length() const
{
   return(buflen_);
}

inline unsigned int DataBlockStrChunk::NumSubGroups() const
{
   return((buflen_ > 0) ? 1 : 0);
}

inline unsigned int
DataBlockStrChunk::NumSubGroups(const LinearExtent &extent) const
{
   if ((extent.Length() > 0) && (extent.Offset() < buflen_)) {
      return(1);
   } else {
      return(0);
   }
}

inline unsigned char &DataBlockStrChunk::operator [](unsigned int bnum) const
{
   assert(bnum < buflen_);
   assert(buf_ != 0);

   return(*(static_cast<unsigned char *>(buf_) + bnum));
}

inline void *DataBlockStrChunk::GetVoidP() const
{
   return(buf_);
}

inline unsigned char *DataBlockStrChunk::GetCharP() const
{
   return(static_cast<unsigned char *>(buf_));
}

inline void DataBlockStrChunk::i_DropUnused(const LinearExtent &usedextent,
					    KeepDir)
{
   unsigned int totlen = usedextent.Offset() + usedextent.Length();

   if (totlen < buflen_) {
      Resize(totlen);
   }
}

#endif
