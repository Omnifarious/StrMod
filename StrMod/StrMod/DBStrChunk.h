#ifndef _STR_DBStrChunk_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/06/29 06:48:54  hopper
// Moved from StrChunk.h and StrChunk.cc along with re-working for new
// StrChunk handling.
//

#include <sys/types.h>
#include <StrMod/StrChunk.h>
#include <StrMod/LinearExtent.h>

#define _STR_DBStrChunk_H_

//--------------------------class DataBlockStrChunk----------------------------

class DataBlockStrChunk : public StrChunk {
 public:
   static const STR_ClassIdent identifier;

   DataBlockStrChunk() : buf_(0), buflen_(0)           { }
   inline DataBlockStrChunk(size_t length);
   inline DataBlockStrChunk(const void *mem, size_t length);
   virtual ~DataBlockStrChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual unsigned int Length() const;
   inline virtual unsigned int NumSubGroups() const;
   inline virtual unsigned int NumSubGroups(const LinearExtent &extent) const;
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   virtual unsigned char &operator [](unsigned int bnum) const;
   inline void *GetVoidP() const;
   inline unsigned char *GetCharP() const;

   void Resize(size_t newsize);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual void i_DropUnused(const LinearExtent &usedextent, KeepDir keepdir);

 private:
   void *buf_;
   size_t buflen_;

   void operator =(const DataBlockStrChunk &b);
   DataBlockStrChunk(const DataBlockStrChunk &b);
};

//--------------------inline functions for DataBlockStrChunk-------------------

inline DataBlockStrChunk::DataBlockStrChunk(unsigned int length)
     : buf_(0), buflen_(0)
{
   Resize(length);
}

inline DataBlockStrChunk::DataBlockStrChunk(const void *mem,
					    unsigned int length)
     : buf_(0), buflen_(0)
{
   Resize(length);
   if (buflen_ > 0) {
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
