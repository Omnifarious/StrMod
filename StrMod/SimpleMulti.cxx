/* $Header$ */

// $Log$
// Revision 1.2  1996/08/31 15:45:03  hopper
// Fixed several bugs in class SimpleMultiplexer that caused it to lock up
// when searching for the plug list.
//
// Revision 1.1  1996/08/24 12:55:52  hopper
// New source module for SimpleMultiplexer class, and related plug classes.
//

#ifdef __GNUG__
#  pragma implementation "SimpleMulti.h"
#endif

#include <StrMod/SimpleMulti.h>
#include <algo.h>

const STR_ClassIdent SimpleMultiplexer::identifier(21UL);
const STR_ClassIdent SimpleMultiplexer::MultiPlug::identifier(22UL);
const STR_ClassIdent SimpleMultiplexer::SinglePlug::identifier(23UL);

SimpleMultiplexer::SimpleMultiplexer()
     : splug_(this), splug_created_(false), multis_left_to_read_mchunk_(0)
{
   nextplug_ = mplugs_.end();
}

SimpleMultiplexer::~SimpleMultiplexer()
{
   splug_.UnPlug();
   while (mplugs_.size() > 0) {
      MultiPlug *mplug = mplugs_.front();

      mplugs_.pop_front();
      mplug->UnPlug();
      delete mplug;
   }
}

bool SimpleMultiplexer::OwnsPlug(StrPlug *plug) const
{
   const StrPlug * const mysplug = &splug_;

   if ((plug == mysplug) && splug_created_) {
      return(true);
   } else {
      return(find(mplugs_.begin(), mplugs_.end(), plug) != mplugs_.end());
   }
}

bool SimpleMultiplexer::DeletePlug(StrPlug *plug)
{
   if ((plug == &splug_) && splug_created_) {
      splug_.UnPlug();
      splug_created_ = false;
      return(true);
   } else {
      MPlugList::iterator search = find(mplugs_.begin(), mplugs_.end(), plug);

      if (search != mplugs_.end()) {
	 MultiPlug *dplug = *search;

	 if ((search == nextplug_) || (dplug == *nextplug_)) {
	    ++nextplug_;
	    if (nextplug_ == mplugs_.end()) {
	       nextplug_ = ((mplugs_.size() == 1) ?
			    mplugs_.end() : mplugs_.begin());
	    }
	 }
	 mplugs_.erase(search);
	 if (!(dplug->HasRead())) {
	    dplug->SetHasRead();
	    --multis_left_to_read_mchunk_;
	    (*search)->UnPlug();
	    if (multis_left_to_read_mchunk_ == 0) {
	       mchunk_.ReleasePtr();
	       splug_.DoWriteableNotify();
	    }
	 } else {
	    (*search)->UnPlug();
	 }
	 delete dplug;
	 return(true);
      } else {
	 return(false);
      }
   }
}

StrPlug *SimpleMultiplexer::CreatePlug(int side)
{
   if ((side == SingleSide) && !splug_created_) {
      splug_created_ = true;
      return(&splug_);
   } else if (side == MultiSide) {
      MultiPlug *nplug = new MultiPlug(this);

      if (mchunk_) {
	 nplug->SetHasRead();
      } else {
	 nplug->ClearHasRead();
      }
      mplugs_.push_back(nplug);
      if (mplugs_.size() == 1) {
	 nextplug_ = mplugs_.begin();
      }
      return(nplug);
   }
}


#include <StrMod/StrChunk.h>

bool SimpleMultiplexer::SingleWrite(const StrChunkPtr &chnk)
{
   if (SingleCanWrite()) {
      MPlugList::iterator i;

      mchunk_ = chnk;
      multis_left_to_read_mchunk_ = mplugs_.size();

      for (i = mplugs_.begin(); i != mplugs_.end(); ++i) {
	 if ((*i)->PluggedInto()) {
	    (*i)->ClearHasRead();
	 } else {
	    (*i)->SetHasRead();
	    --multis_left_to_read_mchunk_;
	 }
      }
//      cerr << "SingleWrite: chnk->Length() == " << chnk->Length()
//	   << " &&  multis_left_to_read_mchunk_ == "
//	   << multis_left_to_read_mchunk_ << "\n";
      for (i = mplugs_.begin(); i != mplugs_.end(); ++i) {
	 if (!((*i)->HasRead())) {
	    (*i)->DoReadableNotify();
	 }
      }
      return(true);
   } else {
      return(false);
   }
}

bool SimpleMultiplexer::SingleCanRead()
{
//   cerr << "SingleCanRead() CALL\n";
   if (mplugs_.size() < 1) {
      return(false);
   }

   MPlugList::iterator newplug(nextplug_);
   MPlugList::iterator end(mplugs_.end());

   if (newplug == end) {
      newplug = mplugs_.begin();
   }
      
   while (true) {
//      cerr << "SingleCanRead() LOOP\n";
      MultiPlug *cur = *newplug;
      
      if (cur->PluggedInto() && cur->PluggedInto()->CanRead()) {
	 nextplug_ = newplug;
//	 cerr << "SingleCanRead() -> true\n";
	 return(true);
      }
      ++newplug;
      if (newplug == nextplug_) {
//	 cerr << "SingleCanRead() -> false\n";
	 return(false);
      }
      if (newplug == end) {
	 newplug = mplugs_.begin();
      }
      if (newplug == nextplug_) {
//	 cerr << "SingleCanRead() -> false\n";
	 return(false);
      }
   }
}

const StrChunkPtr SimpleMultiplexer::SingleRead()
{
   StrChunkPtr retptr;  // Auto-initialized to null.

   if (SingleCanRead()) {
      retptr = (*nextplug_)->PluggedInto()->Read();
//      cerr << "SingleRead: Reading "
//	   << (retptr ? retptr->Length() : 0) << " bytes\n";
      if (++nextplug_ == mplugs_.end()) {
	 nextplug_ = mplugs_.begin();
      }
   }

   return(retptr);
}


bool SimpleMultiplexer::MultiWrite(const StrChunkPtr &ptr)
{
   if (MultiCanWrite()) {
      if (SingleCanWrite()) {
//	 cerr << "MultiWrite: SingleCanWrite\n";
//	 cerr << "(bool)mchunk_ == " << (mchunk_ ? true : false) << "\n";
      }
//      cerr << "MultiWrite: Writing " << ptr->Length() << " bytes\n";
      return(splug_.PluggedInto()->Write(ptr));
   } else {
      return(false);
   }
}

const StrChunkPtr SimpleMultiplexer::MultiRead(MultiPlug *mplug)
{
   StrChunkPtr retptr;  // Auto-initialized to null.
   void *temp = mplug;

//   cerr << "MultiRead:" << temp << ": multis_left_to_read_mchunk_ == "
//	<< multis_left_to_read_mchunk_ << "\n";
   if (MultiCanRead(mplug)) {
      retptr = mchunk_;
      mplug->SetHasRead();
      if (--multis_left_to_read_mchunk_ == 0) {
	 mchunk_.ReleasePtr();
	 splug_.DoWriteableNotify();
      }
   }
   return(retptr);
}
