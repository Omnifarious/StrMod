#ifndef _STR_GroupVector_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/05/08 10:56:32  hopper
// Initial functional revision
//

#include <LCore/Object.h>
#include <LCore/GenTypes.h>
#include <StrMod/STR_ClassIdent.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <assert.h>

#define _STR_GroupVector_H_

typedef U4Byte GV_Size;
class LinearExtent;

class GroupVector : public Object {
 public:
   class Iterator;
   friend class Iterator;

   struct IOVecDesc {
      iovec *iov;
      int iovcnt;
   };

   static const STR_ClassIdent identifier;

   GroupVector(GV_Size size);
   virtual ~GroupVector();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   GV_Size NumGroups() const                          { return(numgroups_); }
   size_t TotalLength() const                         { return(totallength_); }

   inline const void *GetCVoidP(GV_Size group_num) const;
   inline const size_t GetLength(GV_Size group_num) const;
   inline void SetVec(GV_Size group_num, const void *data, size_t length);

   bool FillIOVecDesc(size_t offset, IOVecDesc &desc);
   void FreeAllIOVecDescs();

   Iterator begin();
   Iterator end();

 protected:
   virtual const ClassIdent *i_GetIdent() const       { return(&identifier); }

   void AddIterator()                                 { numiterators_++; }
   void RemoveIterator()                              { numiterators_--; }

 private:
   iovec *groups_;
   GV_Size numgroups_;
   size_t totallength_;
   bool fakegroup_;
   GV_Size fakegroupnum_;
   iovec realgroupdata_;
   size_t lastoffset_;
   GV_Size lastgroupnum_;
   U4Byte numiterators_;
};

//-----------------------------inline functions--------------------------------

inline int GroupVector::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

inline const void *GroupVector::GetCVoidP(GV_Size group_num) const
{
   assert(group_num < NumGroups());

   return(groups_[group_num].iov_base);
}

inline const size_t GroupVector::GetLength(GV_Size group_num) const
{
   assert(group_num < NumGroups());

   return(groups_[group_num].iov_len);
}

inline void GroupVector::SetVec(GV_Size group_num,
				const void *data, size_t length)
{
   assert(group_num < NumGroups());
   assert(length > 0);

   groups_[group_num].iov_base = static_cast<caddr_t>(data);
   groups_[group_num].iov_len = length;
}

#endif
