#ifndef _STR_CharChopper_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../Changelog
//
// Revision 1.1  1996/09/02 23:28:27  hopper
// Added CharChopper class so users would have a simple class that would
// break up and recombine streams using whatever character they chose as
// a separator.
//

#include <StrMod/StreamProcessor.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/DBStrChunk.h>
#include <StrMod/GroupChunk.h>
#include <cstddef>
#include <cassert>

#define _STR_CharChopper_H_

class GroupVector;

class CharChopper : public StreamProcessor {
 public:
   static const STR_ClassIdent identifier;

   CharChopper(char chopchar) : chopchar_(chopchar)    { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   const char chopchar_;
   StrChunkPtrT<GroupChunk> groupdata_;
   StrChunkPtrT<DataBlockStrChunk> curdata_;
   size_t usedsize_;
   enum { INYes, INNo, INMaybe } incoming_is_db_;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual void processIncoming();

   void addChunk(const StrChunkPtr &chnk);
   inline void checkIncoming();
   inline void zeroIncoming();
   inline void replaceIncoming(const StrChunkPtr &data);

 private:
   CharChopper(const CharChopper &b);
};

//-----------------------------inline functions--------------------------------

inline int CharChopper::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamProcessor::AreYouA(cid));
}

inline void CharChopper::checkIncoming()
{
   assert(incoming_);
   if (incoming_is_db_ == INMaybe)
   {
      if (incoming_->AreYouA(DataBlockStrChunk::identifier))
      {
	 incoming_is_db_ = INYes;
      }
      else
      {
	 incoming_is_db_ = INNo;
      }
   }
}

inline void CharChopper::zeroIncoming()
{
   incoming_.ReleasePtr();
   incoming_is_db_ = INMaybe;
}

inline void CharChopper::replaceIncoming(const StrChunkPtr &data)
{
   incoming_ = data;
}

#endif
