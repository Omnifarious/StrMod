#ifndef _STR_StreamModule_H_  // -*- mode: c++ ; c-file-style: "hopper" -*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log information, see ../ChangeLog

#include <cassert>
#include <cstddef>

#include <LCore/Protocol.h>

#ifndef _STR_STR_ClassIdent_H_
#   include <StrMod/STR_ClassIdent.h>
#endif

#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif

#define _STR_StreamModule_H_

class StrChunkPtr;

//: An abstract base for objects that can be modules in the StreamModule
//: framework.

// <P>A StreamModule module can be thought of as a processing element in a
// stream of data.  A StreamModule's 'sides' represent ports at which data can
// enter or leave the module.</P>

// <P>If you were to think of the UNIX utility 'grep' as a StreamModule, it's
// 0 side would be stdin, and it's 1 side would be stdout.  Depending on the
// implementation, it might also have a 2 side, where error messages are
// sent.</P>

// <P>Of course, grep's 'sides' are not bi-directional, they are
// uni-directional.  As a rule, a StreamModule's data port (a Plug) is
// bi-directional.  One way to encapsulate a UNIX program designed to run in a
// pipeline would be to make a module that has a 0 side (stdin) a 1 side, with
// it either passing data unchanged in the reverse direction, or refusing to
// pass data in the reverse direction at all.</P>

// <P>Another way would be to model it as a single sided StreamModule with
// it's input and output happening on the same plug.  This would sort of
// violate the spirit of the StreamModule system though.  There's currently
// only one module which operates this way, the EchoModule, which merely
// echoes all of its input to its output.</P>

class StreamModule : public Protocol {
 public:
   class Plug;
   friend class Plug;
   //: The Strategy for what to do when a plug is disconnected from another
   //: plug.
   // <p>Probably best used as a Flyweight.</p>
   // <p>Not called on module deletion, or if the module thinks it handles its
   // plugs disconnecting and there's no room for a Strategy to get
   // involved.</p>
   class PlugDisconnectStrategy {
    public:
      virtual ~PlugDisconnectStrategy()  { }

      //: Called by a module when one of its plugs has been disconnected.
      virtual void plugDisconnected(StreamModule *m, Plug *p) = 0;
   };

   static const STR_ClassIdent identifier;

   StreamModule() : pdstrategy_(NULL)    { }
   virtual ~StreamModule()               { pdstrategy_ = 0; }

   inline virtual int AreYouA(const ClassIdent &id) const;

   //: Can a plug be created on the given side?
   virtual bool canCreate(int side) const = 0;

   //: Attempts to create a plug on the given side.
   // <p>When writing a derived class, your MakePlug function should ALWAYS
   // call CanCreate first before calling i_MakePlug.</p>
   // <p>Returns 0 (NULL) on failure.  Only fails if CanCreate would return
   // false.</p>
   inline Plug *makePlug(int side);

   //: Does the module own this plug?
   virtual bool ownsPlug(const Plug *plug) const = 0;

   //: Please delete this plug.
   // <p>Modules are supposed to own plugs, so you aren't supposed to delete
   // them yourself.</p>
   // <p>Returns false on failure.  Failure could happen because module does
   // not own the plug.</p>
   virtual bool deletePlug(Plug *plug) = 0;

   //: Get the current PlugDisconnectStrategy.
   // <p>Deleting the object you get this way is a BAD thing unless you set a
   // new strategy before you do it.</p>
   inline PlugDisconnectStrategy *getPDStrategy() const;
   //: Set the current PlugDisconnectStrategy.
   // <p>The StreamModule does not assume responsibility for deleting the
   // object you pass in.  Of course, if you delete it while the StreamModule
   // has it set, that will probably cause bad things to happen.</p>
   // <p>Setting the strategy to NULL essentially turns it off.</p>
   inline void setPDStrategy(PlugDisconnectStrategy *pds);

 protected:
   //: See base class
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //: Called whenever a plug is disconnected.
   // <p><b>This function isn't called when a plug is disconnected as a result
   // of its destructor being called.</b></p>
   // <p>This is used to provide a hook to do what you think you should do
   // when a plug is disconnected.</p>
   // <p>It calls the PDStrategy by default, if there is one.</p>
   inline virtual void plugDisconnected(Plug *plug);

   //: Makes a plug on the given side.
   // Guaranteed to never be called if canCreate would return false.  Must
   // NEVER return 0 (NULL).
   virtual Plug *i_MakePlug(int side) = 0;

   //: Used to set the readable flag of a plug since their set method is
   //: protected.
   inline void setReadableFlagFor(Plug *p, bool val);
   //: Used to set the writeable flag of a plug since their set method is
   //: protected.
   inline void setWriteableFlagFor(Plug *p, bool val);

 private:
   PlugDisconnectStrategy *pdstrategy_;
};

//: A point of connection between one StreamModule and another.

class StreamModule::Plug : public Protocol {
 public:
   friend class StreamModule;
   static const STR_ClassIdent identifier;

   inline Plug(StreamModule &parent);
   inline virtual ~Plug();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: Can this plug be read from?
   bool isReadable() const   { return(flags_.canread_ && !flags_.isreading_); }
   //: Can this plug be written to?
   bool isWriteable() const  { return(flags_.canwrite_ && !flags_.iswriting_); }

   //: Plug this plug into another.  Can fail if already plugged in.
   // <p>A pointer to <code>other</code> will be stored by the plug until it's
   // unplugged or destroyed.  <code>other</code> is a reference because it's
   // not allowed to be NULL.</p>
   inline bool plugInto(Plug &other);
   //: Unplug this plug from any plugs it may be connected to.
   inline void unPlug();
   //: Which plug (if any) is this plug plugged into?  Returns NULL if not
   //: plugged in.
   inline Plug *pluggedInto() const;

   //: Which module owns this plug?
   StreamModule &getParent() const            { return(parent_); }

   //: Which 'side' is this plug on?
   virtual int side() const = 0;

 protected:
   struct Flags {
      bool isreading_ : 1;
      bool iswriting_ : 1;
      bool canread_   : 1;
      bool canwrite_  : 1;
      bool notifyonread_  : 1;
      bool notifyonwrite_ : 1;
   };

   //: See base class
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //: Set whether this plug is readable or not.
   // Even if this is set, isReadable can return false because the plug is
   // already being read from, and a plug can't be read by two things at once.
   inline void setReadable(bool val);
   //: Set whether this plug is writeable or not.
   // <p>Even if this is set, isWriteable can return false because the plug is
   // already being written to, and a plug can't be written to by two things
   // at once.</p>
   inline void setWriteable(bool val);

   //: These are so derived classes have access to this flag.
   void setIsReading(bool val)                { flags_.isreading_ = val; }
   //: These are so derived classes have access to this flag on other plugs.
   static void setIsReading(Plug &othr, bool v)        { othr.setIsReading(v); }
   //: These are so derived classes have access to this flag.
   void setIsWriting(bool val)                { flags_.iswriting_ = val; }
   //: These are so derived classes have access to this flag on other plugs.
   static void setIsWriting(Plug &othr, bool v)        { othr.setIsWriting(v); }

   //: This is so derived classes can get read access to the flags of any plug.
   inline static const Flags &getFlagsFrom(const Plug &p);

   //: Tell a connected plug that this plug's readable state has changed.
   // <p>This checks the flags first and doesn't notify if the other plug
   // doesn't request notification.</p>
   inline void notifyOtherReadable() const;
   //: Tell a connected plug that this plug's readable state has changed.
   // <p>This checks the flags first and doesn't notify if the other plug
   // doesn't request notification.</p>
   inline void notifyOtherWriteable() const;

   //: If this plug needs to be notified of the other's readable state.
   // <p>The only plugs that should really need this are ones for
   // modules where the readable or writeable state of certain plugs
   // depends on the readable state of the plug that this plug is
   // plugged into.</p>
   virtual bool needsNotifyReadable() const            { return(false); }
   //: If this plug needs to be notified of the other's writeable state.
   // <p>The only plugs that should really need this are ones for
   // modules where the readable or writeable state of certain plugs
   // depends on the writeable state of the plug that this plug is
   // plugged into.</p>
   virtual bool needsNotifyWriteable() const           { return(false); }

   //: The 'other plug's readable state changed'  notification function.
   // <p>Called when plug this plug is plugged into changes whether or not it
   // is readable.</p>
   virtual void otherIsReadable()                      { }
   //: The 'other plug's writeable state changed' notification function.
   // <p>Called when plug this plug is plugged into changes whether or not it
   // is writeable.</p>
   virtual void otherIsWriteable()                     { }

   //: Read from this plug.  Behavior is undefined if this plug not readable.
   // <p>When writing your own read function that calls this one, you are
   // responsible for making sure the the plug is readable before calling this
   // function.</p>
   // <p>These functions do not set the isreading or iswriting flags.  You'll
   // need to write wrapper functions that do this.  The pushLoop and pullLoop
   // functions already set the isreading and iswriting flags.</p>
   virtual const StrChunkPtr i_Read() = 0;
   //: Write to this plug.  Behavior is undefined if this plug not writeable.
   // <p>When writing your own write function that calls this one, you are
   // responsible for making sure the the plug is writeable before calling
   // this function.</p>
   // <p>These functions do not set the isreading or iswriting flags.  You'll
   // need to write wrapper functions that do this.  The pushLoop and pullLoop
   // functions already set the isreading and iswriting flags.</p>
   virtual void i_Write(const StrChunkPtr &ptr) = 0;

   //: This is so derived classes can call write on the connected plug.
   void writeOther(const StrChunkPtr &ptr) const   { other_->i_Write(ptr); }
   //: This is so derived classes can call read on the connected plug.
   const StrChunkPtr readOther() const             { return(other_->i_Read()); }

   //: Read as much as possible from this plug and write to the connected plug.
   // <p>This keeps the isreading and iswriting flags set until the loop
   // exits.</p>
   // <p>This loop can exit for a number of reasons, including disconnection,
   // one side 'drying up' and having nothing to read, or one side 'filling
   // up' and not being able to be written to.</p>
   void pushLoop();
   //: Read as much as possible from the connected plug and write to this plug.
   // <p>This keeps the isreading and iswriting flags set until the loop
   // exits.</p>
   // <p>This loop can exit for a number of reasons, including disconnection,
   // one side 'drying up' and having nothing to read, or one side 'filling
   // up' and not being able to be written to.</p>
   void pullLoop();

 private:
   Flags flags_;
   Plug *other_;
   StreamModule &parent_;
};

//-----------------------------inline functions--------------------------------

inline int StreamModule::AreYouA(const ClassIdent &id) const
{
   return((identifier == id) || Protocol::AreYouA(id));
}

inline StreamModule::Plug *StreamModule::makePlug(int side)
{
   if (canCreate(side)) {
      Plug *plug = i_MakePlug(side);

      assert(plug != 0);
      return(plug);
   } else {
      return(0);
   }
}

inline StreamModule::PlugDisconnectStrategy *StreamModule::getPDStrategy() const
{
   return(pdstrategy_);
}

inline void StreamModule::setPDStrategy(PlugDisconnectStrategy *pds)
{
   pdstrategy_ = pds;
}

inline void StreamModule::plugDisconnected(Plug *plug)
{
   if (pdstrategy_ != NULL)
   {
      pdstrategy_->plugDisconnected(this, plug);
   }
}

inline void StreamModule::setReadableFlagFor(Plug *p, bool val)
{
   p->setReadable(val);
}

inline void StreamModule::setWriteableFlagFor(Plug *p, bool val)
{
   p->setWriteable(val);
}

//---

inline StreamModule::Plug::Plug(StreamModule &parent)
     : other_(NULL),
       parent_(parent)
{
   flags_.isreading_ = flags_.iswriting_ = flags_.canread_ = flags_.canwrite_
      = flags_.notifyonread_ = flags_.notifyonwrite_ = false;
}

inline StreamModule::Plug::~Plug()
{
   if (other_ != NULL)
   {
      Plug *temp = other_;

      // This prevents the unPlug routine from gaining control.  It will think
      // this plug is already unplugged.
      other_ = NULL;
      flags_.notifyonread_ = flags_.notifyonwrite_ = false;
      temp->unPlug();
      // Note that parent->plugDisconnected() isn't called here.  This is to
      // avoid calling a partially destructed parent, and various similar
      // kinds of nastiness.
   }
}

inline int StreamModule::Plug::AreYouA(const ClassIdent &id) const
{
   return((identifier == id) || Protocol::AreYouA(id));
}

inline void StreamModule::Plug::unPlug()
{
   // Only need to unplug if we're plugged in.
   if (other_ != NULL)
   {
      Plug *temp = other_;

      // This prevents infinite recursion from happening when the other plug
      // tells us to unplug.  We'll think we're already unplugged.
      other_ = NULL;
      flags_.notifyonread_ = flags_.notifyonwrite_ = false;
      temp->unPlug();
      getParent().plugDisconnected(this);
   }
}

inline StreamModule::Plug *StreamModule::Plug::pluggedInto() const
{
   return(other_);
}

inline void StreamModule::Plug::setReadable(bool val)
{
   bool oldflag = flags_.canread_;

   flags_.canread_ = val;

   if (isReadable() && pluggedInto() != NULL)
   {
      pushLoop();
   }
   if (flags_.canread_ != oldflag)
   {
      notifyOtherReadable();
   }
}

inline void StreamModule::Plug::setWriteable(bool val)
{
   bool oldflag = flags_.canwrite_;

   flags_.canwrite_ = val;

   if (isWriteable() && pluggedInto() != NULL)
   {
      pullLoop();
   }
   if (flags_.canwrite_ != oldflag)
   {
      notifyOtherWriteable();
   }
}

inline const StreamModule::Plug::Flags &
StreamModule::Plug::getFlagsFrom(const Plug &p)
{
   return(p.flags_);
}

inline void StreamModule::Plug::notifyOtherReadable() const
{
   if (flags_.notifyonread_)
   {
      assert(other_ != NULL);
      other_->otherIsReadable();
   }
}

inline void StreamModule::Plug::notifyOtherWriteable() const
{
   if (flags_.notifyonwrite_)
   {
      assert(other_ != NULL);
      other_->otherIsWriteable();
   }
}

#endif
