#ifndef _STR_SimpleMulti_H_  // -*- mode: c++; c-file-style: "hopper"; -*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log, see ../ChangeLog

#include <list>
#include <StrMod/StreamModule.h>

#define _STR_SimpleMulti_H_

namespace strmod {
namespace unievent {
class Dispatcher;
};
};

namespace strmod {
namespace strmod {

/** \class SimpleMultiplexer SimpleMulti.h StrMod/SimpleMulti.h
 * \brief Use this module of you need one source copied to many
 * destinations, and/or many streams combined into one.
 *
 * This implements a simple multiplexer that duplicates all input on the
 * 'single' side to all the plugs attached to the 'multi' side, and combines all
 * the data from the 'multi' side into one flow on the 'single' side.
 *
 * A 'multi' side plug that isn't plugged in is considered to be dead, and no
 * data is routed to it.  This prevents the stream from backing up.
 *
 * Currently, 'multi' side plug deletion is implemented by simply unplugging the
 * plug.  Please don't take advantage of this detail.  In the future, 'multi'
 * side plugs will be actually deleted.
 */
class SimpleMultiplexer : public StreamModule
{
 protected:
   class MultiPlug;
   friend class MultiPlug;
   class SinglePlug;
   friend class SinglePlug;

 public:
   //! The two sides available in a SimpleMultiplexer
   enum PublicSides {
      SingleSide,  //!< Writing to this plug writes to all plugs connected to MultiSide plugs.
      MultiSide  //!< Writing to his plug writes to the plug connected to the SingleSide plug.
   };

   /** Construct a SimpleMultiplexer
    *
    * The strmod::unievent::Dispatcher is needed for making sure data from all
    * MultiSide plugs is handled fairly.  Whenever the SimpleMultiplexer gets
    * data from a MultiPlug, it flags that plug as non-writeable and posts an
    * event to a strmod::unievent::Dispatcher.  When that event is fired, it
    * resets all MultiPlugs to being writeable again.  The prevens any MultiPlug
    * from monopolizing the SinglePlug.
    *
    * @param disp The strmod::unievent::Dispatcher to post to.  */

   SimpleMultiplexer(unievent::Dispatcher &disp);
   //! Also destroys all Plug's and any unsent data.
   virtual ~SimpleMultiplexer();

   inline virtual bool canCreate(int side) const;
   virtual bool ownsPlug(const Plug *plug) const;
   virtual bool deletePlug(Plug *plug);

 protected:
   class SinglePlug : public Plug {
      friend class SimpleMultiplexer;
      friend class MultiPlug;
    public:
      //: Which module owns this plug?
      inline SimpleMultiplexer &getParent() const;

      //: What side is this plug on?
      virtual int side() const                          { return(SingleSide); }

    protected:
      inline SinglePlug(SimpleMultiplexer &parent);
      inline virtual ~SinglePlug();

      //: See base class.
      virtual const StrChunkPtr i_Read();

      //: See base class.
      virtual void i_Write(const StrChunkPtr &ptr);

      //: Because I try to be a 'pass-through' module.
      virtual bool needsNotifyWriteable() const         { return(true); }
      //: Not really a 'pass-through' read module.
      virtual bool needsNotifyReadable() const          { return(false); }

      //: Rather complicated, see the long explanation.
      //
      // <p>If other is writeable, and there is no pending 'update writeable
      // status' event, update all multi-plugs to be writeable.</p>
      // <p>If other isn't writeable, then update all multi-plugs to be
      // non-writeable.</p>
      virtual void otherIsWriteable();
   };

   /** Called whenever a plug is disconnected.
    * Used here to record a MultiPlug as having read the mchunk_ if it's
    * disconnected while it's waiting to be read from.
    *
    * Also calls StreamModule::plugDisconnected so that PDstrategy handling and
    * things can be done there.
    */
   inline virtual void plugDisconnected(Plug *plug);

   virtual Plug *i_MakePlug(int side);

   /** If a scan event isn't posted, post one.
    * A scan is always posted because a piece of data came into a MultiPlug, and
    * data is only allowed to into a MultiPlug once per scan.
    *
    * Data coming into a plug is also cause for that plug to be considered last
    * when checking for data to pull in through a plug.
    */
   inline void postScan(MultiPlug &toend);
   //! Post a scan event to dispatcher_.
   void doPost();
   //! Move a MultiPlug to the end of the list.
   void moveToEnd(MultiPlug &toend);

   //! Process the fact that a MultiPlug read the mchunk_.
   void multiDidRead(MultiPlug &mplug);

   /** \name Count adjusting functions.
    * These functions maintain a count of MultiPlugs that are in a particular
    * state.  The counts are used for adjusting the SingPlug's readable or
    * writeable state.  This provides a way for the plugs, which don't directly
    * know about eachother, to communicate.
    *
    * adjustMultiWriteables adjusts the number of MultiPlugs who's partners are
    * writeable.  If there are none, the SinglePlug's state is changed to not be
    * writeable.  A write to a SinglePlug where there are no MultiPlug partners
    * to forward that data onto would be counterproductive and introduce
    * unecessary buffering.
    *
    * adjustMultiReadables adjusts the number of MultiPlugs who's partners are
    * readable.  If there are no such MultiPlugs, then the SinglePlug won't be
    * able to be read from.  If there is even one MultiPlug who's partner is
    * readable, then a read from the SinglePlug will read from that MultiPlug's
    * partner, so the SinglePlug should be set to readable.
    */
   //@{
   //! Use to say which direction an adjustment should go in.
   enum AdjDir {
      ADJ_Down,  //< Lower the number.
      ADJ_Up  //< Raise the number.
   };

   /** Adjust the number of MultiPlugs who's partners are in a readable state.
    * This may cause the SinglePlug's readable state to change.
    */
   void adjustMultiReadables(AdjDir dir);
   /** Adjust the number of MultiPlugs who's partners are in a writeable state.
    * This may cause the SinglePlug's writeable state to change.
    */
   void adjustMultiWriteables(AdjDir dir);
   //@}

   /** This is called by ScanEvent when its triggerEvent method is called.
    * This resets the 'haswritten' flags of all the MultiPlugs to false.
    *
    * This flag is used to ensure that one MultiPlug can't monopolize the
    * SinglePlug.  A MultiPlug that has written is not allowed to write again
    * until all other MultiPlugs also have a chance to write.
    */
   void doScan();

 private:
   typedef std::list<MultiPlug *> MPlugList;
   class mpother_readable_p;
   friend class mpother_readable_p;
   class mp_notpluggedin_p;
   class mp_written_p;
   friend class mp_written_p;
   class auto_mpptr;
   friend class auto_mpptr;
   class ScanEvent;
   friend class ScanEvent;

   SinglePlug splug_;
   bool splug_created_;
   MPlugList mplugs_;
   MPlugList delplugs_;
   bool scan_posted_;
   StrChunkPtr mchunk_;
   const ::std::tr1::shared_ptr<ScanEvent> scan_;
   unievent::Dispatcher &dispatcher_;
   unsigned int readable_multis_;
   unsigned int readable_multiothers_;
   unsigned int writeable_multiothers_;
};

//-----------------------------inline functions--------------------------------

inline bool SimpleMultiplexer::canCreate(int side) const
{
   if (side == SingleSide) {
      return(!splug_created_);
   } else if (side == MultiSide) {
      return(true);
   } else {
      return(false);
   }
}

inline void SimpleMultiplexer::postScan(MultiPlug &toend)
{
   if (!scan_posted_)
   {
      scan_posted_ = true;
      doPost();
   }
   moveToEnd(toend);
}

//--

inline SimpleMultiplexer::SinglePlug::SinglePlug(SimpleMultiplexer &parent)
     : Plug(parent)
{
}

inline SimpleMultiplexer::SinglePlug::~SinglePlug()
{
}

inline SimpleMultiplexer &SimpleMultiplexer::SinglePlug::getParent() const
{
   return(static_cast<SimpleMultiplexer &>(Plug::getParent()));
}

};  // namespace strmod
};  // namespace strmod

#endif
