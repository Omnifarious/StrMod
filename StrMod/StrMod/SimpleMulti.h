#ifndef _STR_SimpleMulti_H_  // -*- mode: c++; c-file-style: "hopper"; -*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log, see ../ChangeLog

#include <StrMod/StreamModule.h>
#include <list>

#define _STR_SimpleMulti_H_

class UNIDispatcher;

//: Use this module of you need one source copied to many destinations.
// This implements a simple multiplexer that duplicates all input on
// the 'single' side to all the plugs attached to the 'multi' side,
// and combines all the data from the 'multi' side into one flow on
// the 'single' side.
//
// A 'multi' side plug that isn't plugged in is considered to be dead,
// and no data is routed to it.  This prevents the stream from backing
// up.
//
// Currently, 'multi' side plug deletion is implemented by simply
// unplugging the plug.  Please don't take advantage of this detail.
// In the future, 'multi' side plugs will be actually deleted.

class SimpleMultiplexer : public StreamModule {
 protected:
   class MultiPlug;
   friend class MultiPlug;
   class SinglePlug;
   friend class SinglePlug;

 public:
   enum PublicSides { SingleSide, MultiSide };
   static const STR_ClassIdent identifier;

   SimpleMultiplexer(UNIDispatcher &disp);
   virtual ~SimpleMultiplexer();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: See base class.
   inline virtual bool canCreate(int side) const;
   //: See base class.
   virtual bool ownsPlug(const Plug *plug) const;
   //: See base class.
   virtual bool deletePlug(Plug *plug);

 protected:
   class SinglePlug : public Plug {
      friend class SimpleMultiplexer;
      friend class MultiPlug;
    public:
      static const STR_ClassIdent identifier;

      inline virtual int AreYouA(const ClassIdent &cid) const;

      //: Which module owns this plug?
      inline SimpleMultiplexer &getParent() const;

      //: What side is this plug on?
      virtual int side() const                          { return(SingleSide); }

    protected:
      inline SinglePlug(SimpleMultiplexer &parent);
      inline virtual ~SinglePlug();

      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

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

   //: Called whenever a plug is disconnected.
   // <P>Used here to record a plug as having read the mchunk_ if it's
   // disconnected while it's waiting to be read from.<P>
   // <P>Also calls StreamModule::plugDisconnected so that PDstrategy handling
   // and things can be done there.</P>
   inline virtual void plugDisconnected(Plug *plug);

   //: Makes a plug on the given side.
   // <P>Guaranteed to never be called if canCreate would return false.  Must
   // NEVER return 0 (NULL).</P>
   virtual Plug *i_MakePlug(int side);


   //: If a scan event isn't posted, post one.
   // <P>A scan is always posted because a piece of data came into a
   // MultiPlug, and data is only allowed to into a MultiPlug once per
   // scan.</P>
   // <P>Data coming into a plug is also cause for that plug to be
   // considered last when checking for data to pull in through a
   // plug.
   inline void postScan(MultiPlug &toend);
   //: Actually post a scan event to the dispatcher given in the constructor.
   void doPost();
   //: Move a MultiPlug to the end of the list.
   void moveToEnd(MultiPlug &toend);

   //: Process the fact that a MultiPlug read the mchunk_.
   void multiDidRead(MultiPlug &mplug);

   //: Use to say which direction an adjustment should go in.
   enum AdjDir { ADJ_Down, ADJ_Up };

   //: This may cause the SinglePlug's readable state to change.
   void adjustMultiReadables(AdjDir dir);
   //: This may cause the SinglePlug's writeable state to change.
   void adjustMultiWriteables(AdjDir dir);

   //: This is called by ScanEvent.
   void doScan();

 private:
   typedef list<MultiPlug *> MPlugList;
   class mpother_readable_p;
   class mp_notpluggedin_p;
   class mp_written_p;
   class auto_mpptr;
   class ScanEvent;
   friend class ScanEvent;

   SinglePlug splug_;
   bool splug_created_;
   MPlugList mplugs_;
   MPlugList delplugs_;
   bool scan_posted_;
   StrChunkPtr mchunk_;
   ScanEvent * const scan_;
   UNIDispatcher &dispatcher_;
   unsigned int readable_multis_;
   unsigned int readable_multiothers_;
   unsigned int writeable_multiothers_;
};

//-----------------------------inline functions--------------------------------

inline int SimpleMultiplexer::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

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

inline int SimpleMultiplexer::SinglePlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline SimpleMultiplexer::SinglePlug::SinglePlug(SimpleMultiplexer &parent)
     : Plug(parent)
{
}

inline SimpleMultiplexer::SinglePlug::~SinglePlug()
{
   unPlug();
}

inline SimpleMultiplexer &SimpleMultiplexer::SinglePlug::getParent() const
{
   return(static_cast<SimpleMultiplexer &>(Plug::getParent()));
}

#endif
