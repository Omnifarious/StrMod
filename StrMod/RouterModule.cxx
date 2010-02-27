/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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

#ifdef __GNUG__
#  pragma implementation "RouterModule.h"
#endif

#include "StrMod/RouterModule.h"
#include <UniEvent/Dispatcher.h>
#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>
#include <algorithm>

namespace strmod {
namespace strmod {

using unievent::Dispatcher;
using unievent::Event;

class RouterModule::ScanEvent : public Event {
 public:
   //: This keeps a reference to parent.
   ScanEvent(RouterModule &parent) : parent_(&parent)   { }

   inline virtual void triggerEvent(Dispatcher *dispatcher = 0);

   void parentGone()                                    { parent_ = 0; }

 private:
   RouterModule *parent_;
};

inline void RouterModule::ScanEvent::triggerEvent(Dispatcher *dispatcher)
{
   if (parent_)
   {
      parent_->doScan();
   }
}

RouterModule::RouterModule(Dispatcher &disp)
     : disp_(disp), scan_posted_(false), scan_(new ScanEvent(*this)),
       inroutingdone_(false), outgoingcopies_(0)
{
}

RouterModule::~RouterModule()
{
   scan_->parentGone();
   while (allplugs_.size() > 0) {
      RPlug *rplug = allplugs_.front();

      allplugs_.pop_front();
      delete rplug;
   }
}

bool RouterModule::ownsPlug(const Plug *plug) const
{
   const RPlugList::const_iterator end(allplugs_.end());
   const RPlugList::const_iterator search(find(allplugs_.begin(), end, plug));
   if (search != end)
   {
      return(true);
   }
   else
   {
      return(false);
   }
}

bool RouterModule::deletePlug(Plug *plug)
{
   const RPlugList &allplugs_l = allplugs_;
   const RPlugList::const_iterator end(allplugs_l.end());
   const RPlugList::const_iterator search(find(allplugs_l.begin(), end, plug));
   if ((search != end) && ! (*search)->getDeleted())
   {
      RPlug * const rp = *search;
      assert(rp == plug);
      rp->setDeleted();
      rp->unPlug();
      postScan();
      return(true);
   }
   else
   {
      return(false);
   }
}

void RouterModule::addNewPlug(RPlug *rp)
{
   assert(!ownsPlug(rp));
   assert(rp->pluggedInto() == NULL);
   assert(this == &(rp->getParent()));
   allplugs_.push_back(rp);
   if (routedchunk_)
   {
      setWriteableFlagFor(rp, false);
      nonwriteable_.push_back(rp);
   }
   else
   {
      setWriteableFlagFor(rp, true);
      writeable_.push_back(rp);
   }
}

void RouterModule::doPost()
{
   disp_.addEvent(scan_);
}

void RouterModule::doScan()
{
   assert(!inroutingdone_);
   scan_posted_ = false;
   writeable_.clear();
   nonwriteable_.clear();
   {
      const size_t size = allplugs_.size();
      for (size_t i = 0; i < size; ++i)
      {
         RPlug * const curplug = allplugs_.front();
         allplugs_.pop_front();
         if (curplug->getDeleted())
         {
            assert(curplug->pluggedInto() == NULL);
            delete curplug;
         }
         else
         {
            allplugs_.push_back(curplug);
            if (RPlug::getFlagsFrom(*curplug).canwrite_)
            {
               writeable_.push_back(curplug);
            }
            else
            {
               nonwriteable_.push_back(curplug);
            }
         }
      }
   }
   assert((outgoingcopies_ > 0 && routedchunk_ && writeable_.size() == 0) ||
          (outgoingcopies_ <= 0 && !routedchunk_));
   if (nonwriteable_.size() > 0)
   {
      routingDone();
   }
}

void RouterModule::routingDone()
{
   assert(outgoingcopies_ == 0 && !routedchunk_);
   if (inroutingdone_)
   {
      return;
   }
   inroutingdone_ = true;
   while (!routedchunk_ && nonwriteable_.size() > 0)
   {
      RPlug *towrite = nonwriteable_.front();
      nonwriteable_.pop_front();
      assert(! RPlug::getFlagsFrom(*towrite).canwrite_);
      writeable_.push_back(towrite);
      setWriteableFlagFor(towrite, true);
   }
   inroutingdone_ = false;
}

void RouterModule::processIncoming(RPlug &source, const StrChunkPtr &chunk)
{
   assert(outgoingcopies_ == 0 && !routedchunk_);
   assert(! source.getFlagsFrom(source).canwrite_);
   routedchunk_ = chunk;
   outgoingcopies_ = 1;
   {
      RPlugList dests;
      {
         const RPlugList &constall = allplugs_;
         RPlugAdder adder(dests);
         getDestinations(chunk, source, constall.begin(), constall.end(),
                         adder);
      } // End scope of constall
      outgoingcopies_ = dests.size();
      if (outgoingcopies_ >= 0)
      {
         while (writeable_.size() > 0)
         {
            RPlug *tononwrite = writeable_.front();
            writeable_.pop_front();
            // Neglect o put source on non-writeable list because it shouldn't
            // be made writeable again until the next scan.  Also don't bother
            // with deleted plugs.
            if ((tononwrite != &source) && ! tononwrite->getDeleted())
            {
               assert(RPlug::getFlagsFrom(*tononwrite).canwrite_);
               nonwriteable_.push_back(tononwrite);
               setWriteableFlagFor(tononwrite, false);
            }
         }
         {
            size_t skippedplugs = 0;

            while (dests.size() > 0)
            {
               assert(routedchunk_);
               assert(outgoingcopies_ > 0);
               RPlug * const routedto = dests.front();
               dests.pop_front();
               assert(routedto->getDeleted() || ownsPlug(routedto));
               if (!routedto->getDeleted() && routedto->pluggedInto() != NULL)
               {
                  setReadableFlagFor(routedto, true);
               }
               else
               {
                  ++skippedplugs;
               }
            }
            if (skippedplugs > 0)
            {
               assert(outgoingcopies_ >= skippedplugs);
               assert(routedchunk_);
               if (skippedplugs < outgoingcopies_)
               {
                  outgoingcopies_ -= skippedplugs;
               }
               else
               {
                  outgoingcopies_ = 0;
               }
               if (outgoingcopies_ <= 0)
               {
                  routedchunk_.reset();
               }
            }
         } // End scope of skippedplugs
      } // End outgoingcopies_ > 0
      else // outgoingcopies_ <= 0
      {
         routedchunk_.reset();
      }
   }  // End scope of dests and all other local variables.
   assert((outgoingcopies_ > 0 && routedchunk_) ||
          (outgoingcopies_ <= 0 && !routedchunk_));
   if (outgoingcopies_ == 0 && !inroutingdone_)
   {
      routingDone();
   }
}

const StrChunkPtr RouterModule::RPlug::i_Read()
{
   assert(getFlagsFrom(*this).canread_);
   assert(getFlagsFrom(*this).isreading_);
   RouterModule &parent = getParent();
   assert(parent.outgoingcopies_ > 0);
   assert(parent.routedchunk_);
   StrChunkPtr tmp = parent.routedchunk_;
   setReadable(false);
   if (--parent.outgoingcopies_ == 0)
   {
      parent.routedchunk_.reset();
      if (!parent.inroutingdone_)
      {
         parent.routingDone();
      }
   }
   return(tmp);
}

void RouterModule::RPlug::i_Write(const StrChunkPtr &ptr)
{
   assert(getFlagsFrom(*this).canwrite_);
   assert(getFlagsFrom(*this).iswriting_);
   RouterModule &parent = getParent();
   assert(!parent.routedchunk_);
   setWriteable(false);
   parent.postScan();
   parent.processIncoming(*this, ptr);
}

};  // End namespace strmod
};  // End namespace strmod
