/*
 * Copyright 1991-2002 Eric M. Hopper <hopper@omnifarious.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Header$ */

// For a log, see ChangeLog

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

#include "StrMod/SimpleMulti.h"
#include "StrMod/StrChunk.h"
#include <UniEvent/Dispatcher.h>
#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>
#include <algorithm>
#include <cassert>

namespace strmod {
namespace strmod {

/** A private class for StrPlugs on the 'many' side of a SimpleMultiplexer.
 */
class SimpleMultiplexer::MultiPlug : public StreamModule::Plug
{
   friend class SimpleMultiplexer;
   friend class SinglePlug;
 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const lcore::ClassIdent &cid) const;

   inline SimpleMultiplexer &getParent() const;

   virtual int side() const                           { return(MultiSide); }

   //! Get the 'has_written_' flag of this plug.
   bool getHasWritten() const                         { return(has_written_); }

   //! Set the 'has_written_' flag of this plug.
   void setHasWritten(bool nval)                      { has_written_ = nval; }

 protected:
   //! A constructor.
   inline MultiPlug(SimpleMultiplexer &parent);
   //! A destructor.  Calls unPlug()
   inline virtual ~MultiPlug();

   virtual const lcore::ClassIdent *i_GetIdent() const  { return &identifier; }

   virtual const StrChunkPtr i_Read();

   virtual void i_Write(const StrChunkPtr &ptr);

   virtual bool needsNotifyWriteable() const        { return(true); }
   virtual bool needsNotifyReadable() const         { return(true); }

   virtual void otherIsReadable();

   virtual void otherIsWriteable();

   /** Set the list position of this plug.
    * This function is to set the listpos_ iterator that's used for quick list
    * operations.  In fact, the primary purpose of this variable is to set it up
    * so the list doesn't have to be scanned to find this plug when the plug
    * needs to be moved to the bottom of the list.
    *
    * Of course, this means that the data structure the plugs are stored in
    * needs to preserve iterators through list operations.
    */
   void setListPos(const MPlugList::iterator &newpos) { listpos_ = newpos; }

   //! Get the list position of this plug.  See setListPos for more details.
   const MPlugList::iterator &getListPos() const    { return(listpos_); }

 private:
   MPlugList::iterator listpos_;
   bool has_written_;
   bool other_iswriteable_;
   bool other_isreadable_;
};

//! The event that is triggered when the SimpleMultiplexer should scan the
//! MultiPlugs for ones that are readable.
class SimpleMultiplexer::ScanEvent : public unievent::Event {
 private:
   typedef unievent::Dispatcher Dispatcher;
 public:
   static const STR_ClassIdent identifier;

   //! This keeps a reference to parent.
   ScanEvent(SimpleMultiplexer &parent) : parent_(&parent)                   { }

   inline virtual void triggerEvent(Dispatcher *dispatcher = 0);

   void parentGone()                                    { parent_ = 0; }

 protected:
   virtual const lcore::ClassIdent *i_GetIdent()        { return &identifier; }

 private:
   SimpleMultiplexer *parent_;
};

int SimpleMultiplexer::MultiPlug::AreYouA(const lcore::ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

SimpleMultiplexer &SimpleMultiplexer::MultiPlug::getParent() const
{
   return(static_cast<SimpleMultiplexer &>(Plug::getParent()));
}

const STR_ClassIdent SimpleMultiplexer::identifier(21UL);
const STR_ClassIdent SimpleMultiplexer::MultiPlug::identifier(22UL);
const STR_ClassIdent SimpleMultiplexer::SinglePlug::identifier(23UL);
const STR_ClassIdent SimpleMultiplexer::ScanEvent::identifier(32UL);

SimpleMultiplexer::MultiPlug::MultiPlug(SimpleMultiplexer &parent)
     : Plug(parent), has_written_(false),
       other_iswriteable_(false), other_isreadable_(false)
{
}

SimpleMultiplexer::MultiPlug::~MultiPlug()
{
   unPlug();
}

const StrChunkPtr SimpleMultiplexer::MultiPlug::i_Read()
{
//     cerr << "SimpleMultiplexer::MultiPlug::i_Read\n";
   SimpleMultiplexer &parent = getParent();

   assert(parent.mchunk_ && (parent.readable_multis_ > 0));

   StrChunkPtr retval = parent.mchunk_;

   parent.multiDidRead(*this);

   return(retval);
}

void SimpleMultiplexer::MultiPlug::i_Write(const StrChunkPtr &ptr)
{
//     cerr << "SimpleMultiplexer::MultiPlug::i_Write\n";
   // Make sure that I'm allowed to be written to.
   assert(!has_written_);
   assert(getFlagsFrom(*this).canwrite_);
   // Make sure that the world knows I'm being written to.
   assert(getFlagsFrom(*this).iswriting_);

   // Set up a local alias for my parent.
   SimpleMultiplexer &parent = getParent();

   // Make sure that the plug I need to write to exists, and can be written to.
   assert((parent.splug_.pluggedInto() != 0)
	  && (parent.splug_.pluggedInto()->isWriteable()));

   // I can't be written to again.
   has_written_ = true;
   setWriteable(false);
   // Treat has_written_ going to true as other.canread_ going to false.
   // This means that my parent module won't count me when trying to decide if
   // splug_.canread should be true or not.
   if (other_isreadable_)
   {
      parent.adjustMultiReadables(ADJ_Down);
   }

   // Make sure that setting those flags didn't somehow alter the other plug's
   // state.
   assert((parent.splug_.pluggedInto() != 0)
	  && (parent.splug_.pluggedInto()->isWriteable()));

   // Get a reference to the other plug.
   Plug &other = *parent.splug_.pluggedInto();

   setIsWriting(other, true);
   // Preserve the old value of the isreading_ flag so it can be restored.
   //
   // I don't need to preserve the previous flag because I can be sure the
   // other's iswriting_ flag is false, because other.isWriteable() returns
   // true.
   bool oldisread = getFlagsFrom(parent.splug_).isreading_;
   setIsReading(parent.splug_, true);
   // Actually do the write.
   parent.splug_.writeOther(ptr);
   // Reset the flags.
   setIsWriting(other, false);
   setIsReading(parent.splug_, oldisread);
   // Tell my parent to post a scan event that will reset my has_written_
   // flag, as well as move me to the end of the list.
   parent.postScan(*this);
}

void SimpleMultiplexer::MultiPlug::otherIsReadable()
{
//     cerr << "SimpleMultiplexer::MultiPlug::otherIsReadable()\n";

   bool newreadable = false;

   if (pluggedInto() == 0)
   {
      newreadable = false;
   }
   else
   {
      newreadable = getFlagsFrom(*(pluggedInto())).canread_;
   }
   if (newreadable != other_isreadable_)
   {
      other_isreadable_ = newreadable;
      // If has_written_, the readable flag makes no difference, and the
      // parent has already been notified of my state.
      if (!has_written_)
      {
	 getParent().adjustMultiReadables(newreadable ? ADJ_Up : ADJ_Down);
      }
   }
}

void SimpleMultiplexer::MultiPlug::otherIsWriteable()
{
//     cerr << "SimpleMultiplexer::MultiPlug::otherIsWriteable()\n";

   bool newwriteable = false;

   if (pluggedInto() == 0)
   {
      newwriteable = false;
   }
   else
   {
      newwriteable = getFlagsFrom(*(pluggedInto())).canwrite_;
   }
   if (newwriteable != other_iswriteable_)
   {
      other_iswriteable_ = newwriteable;
      getParent().adjustMultiWriteables(newwriteable ? ADJ_Up : ADJ_Down);
   }
}

void SimpleMultiplexer::ScanEvent::triggerEvent(Dispatcher *dispatcher)
{
   if (parent_ != 0)
   {
      parent_->doScan();
   }
}

//: Used as a predicate to search for a MultiPlug to read from.
class SimpleMultiplexer::mpother_readable_p {
 public:
   bool operator ()(MultiPlug *p) {
      return(!p->getHasWritten() && (p->pluggedInto() != 0)
	     && p->pluggedInto()->isReadable());
   }
};

const StrChunkPtr SimpleMultiplexer::SinglePlug::i_Read()
{
//     cerr << "SimpleMultiplexer::SinglePlug::i_Read\n";
   SimpleMultiplexer &parent = getParent();
   MPlugList::iterator pptr;
   {
      MPlugList::iterator end = parent.mplugs_.end();
      pptr = find_if(parent.mplugs_.begin(), end,
		     mpother_readable_p());
      assert(pptr != end);
   }
   MultiPlug &sibling = *(*pptr);
   assert(sibling.pluggedInto() != 0);
   assert(sibling.getHasWritten() == false);
   Plug &other = *(sibling.pluggedInto());
   assert(other.isReadable());

   parent.setWriteableFlagFor(&sibling, false);
   sibling.setHasWritten(true);
   parent.postScan(sibling);
   parent.adjustMultiReadables(ADJ_Down);
   assert(other.isReadable());

   setIsReading(other, true);
   StrChunkPtr retval = sibling.readOther();
   setIsReading(other, false);

   return(retval);
}

//: Automatically delete an array when it goes out of scope.
class SimpleMultiplexer::auto_mpptr
{
 public:
   auto_mpptr(MultiPlug **p) : p_(p)  { }
   ~auto_mpptr()                     { delete[] p_; }

 private:
   MultiPlug **p_;
};

//: Find MultiPlugs that are plugged in.
class SimpleMultiplexer::mp_notpluggedin_p {
 public:
   bool operator ()(Plug *p) {
      return(p->pluggedInto() == 0);
   }
};

void SimpleMultiplexer::SinglePlug::i_Write(const StrChunkPtr &chnk)
{
//     cerr << "SimpleMultiplexer::SinglePlug::i_Write\n";
   SimpleMultiplexer &parent = getParent();
   assert(!parent.mchunk_);
   MultiPlug **scanplugs = new MultiPlug *[parent.mplugs_.size()];
   auto_mpptr delary(scanplugs);

   MultiPlug **endscan = remove_copy_if(parent.mplugs_.begin(),
					parent.mplugs_.end(),
					scanplugs,
					mp_notpluggedin_p());
   parent.mchunk_ = chnk;
   parent.readable_multis_ = endscan - scanplugs;
   setWriteable(false);
   for (MultiPlug **curppp = scanplugs; curppp != endscan; ++curppp)
   {
      MultiPlug &curp = **curppp;
      assert(getFlagsFrom(curp).canread_ == false);
      if (curp.pluggedInto() != 0)
      {
//  	 cerr << "Setting a multi readable.\n";
	 curp.setReadable(true);
      }
      else
      {
	 parent.multiDidRead(curp);
      }
   }
}

//! Find MultiPlugs that haven't been written to.
class SimpleMultiplexer::mp_written_p {
 public:
   bool operator ()(MultiPlug *p) {
      return(p->getHasWritten());
   }
};

void SimpleMultiplexer::SinglePlug::otherIsWriteable()
{
//     cerr << "SimpleMultiplexer::SinglePlug::otherIsWriteable\n";
   SimpleMultiplexer &parent = getParent();
   MultiPlug **scanplugs = new MultiPlug *[parent.mplugs_.size()];
   auto_mpptr delary(scanplugs);

   MultiPlug **endscan = remove_copy_if(parent.mplugs_.begin(),
					parent.mplugs_.end(),
					scanplugs,
					mp_written_p());
//     cerr << "Setting " << (endscan - scanplugs) << " plugs.\n";
   for (MultiPlug **curppp = scanplugs; curppp != endscan; ++curppp)
   {
      MultiPlug &curp = **curppp;
      assert(curp.getHasWritten() == false);
      curp.setWriteable(pluggedInto()
			&& getFlagsFrom(*pluggedInto()).canwrite_);
   }
}

SimpleMultiplexer::SimpleMultiplexer(unievent::Dispatcher &disp)
     : splug_(*this), splug_created_(false), scan_posted_(false),
       scan_(new ScanEvent(*this)), dispatcher_(disp),
       readable_multis_(0), readable_multiothers_(0), writeable_multiothers_(0)
{
   scan_->AddReference();
}

SimpleMultiplexer::~SimpleMultiplexer()
{
   scan_->parentGone();
   splug_.unPlug();
   while (mplugs_.size() > 0) {
      MultiPlug *mplug = mplugs_.front();

      mplug->setListPos(mplugs_.end());
      mplugs_.pop_front();
      mplug->unPlug();
      delete mplug;
   }
   scan_->DelReference();
   if (scan_->NumReferences() <= 0)
   {
      delete scan_;
   }
}

bool SimpleMultiplexer::ownsPlug(const Plug *plug) const
{
   const StreamModule::Plug * const p = plug;
   if ((p == &splug_) && splug_created_) {
      return(true);
   } else {
      MPlugList::const_iterator end = mplugs_.end();
      return(find(mplugs_.begin(), end, plug) != end);
   }
}

void SimpleMultiplexer::doPost()
{
   dispatcher_.addEvent(scan_);
}

void SimpleMultiplexer::moveToEnd(MultiPlug &toend)
{
//     cerr << "SimpleMultiplexer::moveToEnd\n";
   MPlugList::iterator oldpos = toend.getListPos();
   toend.setListPos(mplugs_.insert(mplugs_.end(), &toend));
   mplugs_.erase(oldpos);
}

void SimpleMultiplexer::multiDidRead(MultiPlug &mplug)
{
//     cerr << "SimpleMultiplexer::multiDidRead\n";
//     cerr << "readable_multis_ == " << readable_multis_ << "\n";
   assert(readable_multis_ > 0);
   setReadableFlagFor(&mplug, false);

   if (--readable_multis_ == 0)
   {
//        cerr << "readable_multis_ == 0 && writeable_multiothers_ == "
//  	   << writeable_multiothers_ << "\n";
      mchunk_.ReleasePtr();
      if (writeable_multiothers_ > 0)
      {
	 setWriteableFlagFor(&splug_, true);
      }
      else
      {
	 setWriteableFlagFor(&splug_, false);
      }
   }
}

void SimpleMultiplexer::adjustMultiReadables(AdjDir dir)
{
   assert((dir == ADJ_Down) || (dir == ADJ_Up));
   if (dir == ADJ_Down)
   {
      assert(readable_multiothers_ > 0);
      if (--readable_multiothers_ == 0)
      {
	 setReadableFlagFor(&splug_, false);
      }
   }
   else
   {
      assert(readable_multiothers_ < mplugs_.size());
      ++readable_multiothers_;
      setReadableFlagFor(&splug_, true);
   }
}

void SimpleMultiplexer::adjustMultiWriteables(AdjDir dir)
{
   assert((dir == ADJ_Down) || (dir == ADJ_Up));
   if (dir == ADJ_Down)
   {
      assert(writeable_multiothers_ > 0);
      if (--writeable_multiothers_ == 0)
      {
	 setWriteableFlagFor(&splug_, false);
      }
   }
   else
   {
      assert(writeable_multiothers_ < mplugs_.size());
      ++writeable_multiothers_;
      if (!mchunk_)
      {
	 setWriteableFlagFor(&splug_, true);
      }
   }
}

inline void SimpleMultiplexer::plugDisconnected(Plug *plug)
{
   if (plug->side() == MultiSide)
   {
      MultiPlug * const mp = static_cast<MultiPlug *>(plug);

      if (mp->getFlagsFrom(*mp).canread_)
      {
	 multiDidRead(*mp);
      }
      mp->otherIsReadable();
      mp->otherIsWriteable();
   }
   else
   {
      splug_.otherIsWriteable();
   }
}

StreamModule::Plug *SimpleMultiplexer::i_MakePlug(int side)
{
   if ((side == SingleSide) && !splug_created_)
   {
      splug_created_ = true;
      return(&splug_);
   }
   else if (side == MultiSide)
   {
      MultiPlug *mp = new MultiPlug(*this);
      mplugs_.push_front(mp);
      mp->setListPos(mplugs_.begin());
      if (splug_.pluggedInto() != 0)
      {
	 bool writeflag = splug_.getFlagsFrom(*splug_.pluggedInto()).canread_;
	 setWriteableFlagFor(mp, writeflag);
      }
      return(mp);
   }
   else
   {
      assert(false);
      return(0);
   }
}

void SimpleMultiplexer::doScan()
{
//     cerr << "SimpleMultiplexer::doScan\n";

   while (delplugs_.size() > 0)
   {
      MultiPlug * const mp = delplugs_.front();
      assert(mp->pluggedInto() == 0);
      MPlugList::iterator pos = mp->getListPos();
      mp->setListPos(mplugs_.end());
      mplugs_.erase(pos);
      delplugs_.pop_front();
      delete mp;
   }

   scan_posted_ = false;
   MultiPlug **scanplugs = new MultiPlug *[mplugs_.size()];
   auto_mpptr delary(scanplugs);

   MultiPlug **endscan = copy(mplugs_.begin(), mplugs_.end(), scanplugs);
   for (MultiPlug **curppp = scanplugs; curppp != endscan; ++curppp)
   {
      MultiPlug &curp = **curppp;
      if (curp.getHasWritten())
      {
//  	 cerr << "Setting a true has_written to false.\n";
	 assert(splug_.getFlagsFrom(curp).canwrite_ == false);
	 curp.setHasWritten(false);
	 if (curp.pluggedInto()
	     && curp.getFlagsFrom(*curp.pluggedInto()).canread_)
	 {
	    adjustMultiReadables(ADJ_Up);
	 }
      }
      if (!curp.getHasWritten())
      {
	 const bool writeflag = splug_.pluggedInto()
	    && splug_.getFlagsFrom(*splug_.pluggedInto()).canwrite_;
//  	 if (splug_.getFlagsFrom(curp).canwrite_ == false && writeflag)
//  	 {
//  	    cerr << "DS: Going from 0 to 1\n";
//  	 }
	 curp.setWriteable(writeflag);
      }
   }
//     cerr << "------SimpleMultiplexer::doScan\n";
}

bool SimpleMultiplexer::deletePlug(Plug *plug)
{
   if ((&splug_ == plug) && splug_created_)
   {
      splug_.unPlug();
      splug_created_ = false;
      return(true);
   }
   else
   {
      MPlugList::iterator end = mplugs_.end();
      MPlugList::iterator search = find(mplugs_.begin(), end, plug);
      if (search != end)
      {
	 MultiPlug * const mp = *search;
	 assert(mp == plug);
	 mp->unPlug();
	 delplugs_.push_front(mp);
	 postScan(*mp);
	 return(true);
      }
   }
   return false;
}

}  // End namespace strmod
}  // End namespace strmod
