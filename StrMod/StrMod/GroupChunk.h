#ifndef _STR_GroupChunk_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.
// Revision 1.1  1996/06/29 06:51:30  hopper
// New class GroupChunk to hold a group of chunks and make them appear to
// be one chunk.
//

#include <StrMod/StrChunk.h>
#include <deque>

#define _STR_GroupChunk_H_

class StrChunkPtr;

class GroupChunk : public StrChunk {
   typedef deque<StrChunk *> ChunkList;
   class RangeAccumulator;
 public:
   static const STR_ClassIdent identifier;

   GroupChunk();
   virtual ~GroupChunk();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual unsigned int Length() const                 { return(totalsize_); }
   virtual unsigned int NumSubGroups() const;
   virtual unsigned int NumSubGroups(const LinearExtent &extent) const;
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index);

   void push_back(const StrChunkPtr &chnk);
   void push_front(const StrChunkPtr &chnk);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual void i_DropUnused(const LinearExtent &usedextent, KeepDir keepdir);

   void AccumulateRange(const LinearExtent &extent,
			RangeAccumulator &accum) const;

 private:
   class RangeAccumulator {
    public:
      RangeAccumulator()                                { }
      virtual void operator ()(StrChunk &chnk,
			       const LinearExtent &ext) = 0;
    private:
      RangeAccumulator(const RangeAccumulator &);
      void operator =(const RangeAccumulator &b);
   };
   class NumGroupAccumulator : public RangeAccumulator {
    public:
      NumGroupAccumulator() : numgroups_(0)             { }

      virtual void operator ()(StrChunk &chnk, const LinearExtent &ext);

      unsigned int NumGroups() const                    { return(numgroups_); }

    private:
      unsigned int numgroups_;
   };
   class GroupVecAccumulator : public RangeAccumulator {
    public:
      inline GroupVecAccumulator(GroupVector *vec, unsigned int *start_index);

      virtual void operator ()(StrChunk &chnk, const LinearExtent &ext);

    private:
      GroupVector &vec_;
      unsigned int &index_;
   };
      
   ChunkList chnklist_;
   unsigned int totalsize_;

   GroupChunk(const GroupChunk &);
   void operator =(const GroupChunk &);
};

//-----------------------------inline functions--------------------------------

inline int GroupChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrChunk::AreYouA(cid));
}

inline
GroupChunk::GroupVecAccumulator::GroupVecAccumulator(GroupVector *vec,
						     unsigned int *start_index)
     : vec_(*vec), index_(*start_index)
{
}

#endif
