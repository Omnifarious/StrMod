#ifndef _STR_StreamModule_H_  // -*- mode: c++ ; c-file-style: "hopper" -*-

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

namespace strmod {
namespace strmod {

class StrChunkPtr;

/** \class StreamModule StreamModule.h StrMod/StreamModule.h
 * An abstract base for objects that can be modules in the StreamModule
 * framework.
 *
 * A StreamModule module can be thought of as a processing element in a stream
 * of data.  A StreamModule's 'sides' represent ports at which data can enter
 * or leave the module.
 *
 * If you were to think of the UNIX utility 'grep' as a StreamModule, it's 0
 * side would be stdin, and it's 1 side would be stdout.  Depending on the
 * implementation, it might also have a 2 side, where error messages are sent.
 *
 * Of course, grep's 'sides' are not bi-directional, they are uni-directional.
 * As a rule, a StreamModule's data port (a Plug) is bi-directional.  One way
 * to encapsulate a UNIX program designed to run in a pipeline would be to
 * make a module that has a 0 side (stdin) a 1 side, with it either passing
 * data unchanged in the reverse direction, or refusing to pass data in the
 * reverse direction at all.
 *
 * Another way would be to model it as a single sided StreamModule with it's
 * input and output happening on the same plug.  This would sort of violate
 * the spirit of the StreamModule system though.  There's currently only one
 * module which operates this way, the EchoModule, which merely echoes all of
 * its input to its output.  */

class StreamModule : public Protocol {
 public:
   class Plug;
   friend class Plug;
   /**
    * The Strategy for what to do when a plug is disconnected from another
    * plug.
    * 
    * Probably best used as a Flyweight.
    * 
    * Not called on module deletion, or if the module thinks it handles its
    * plugs disconnecting and there's no room for a Strategy to get involved.
    */
   class PlugDisconnectStrategy {
    public:
       /**
        * Here so derived classes have a virtual destructor.
        */
      virtual ~PlugDisconnectStrategy()  { }

      /**
       * Called by a module when one of its plugs has been disconnected.
       *
       * This is not called when a plug is disconnected because the module
       * itself is being destructed.
       */
      virtual void plugDisconnected(StreamModule *m, Plug *p) = 0;
   };

   static const STR_ClassIdent identifier;

   //! Abstract class, doesn't do much.
   StreamModule() : pdstrategy_(NULL)    { }
   //! Prudently virtual in an abstract base class.
   virtual ~StreamModule()               { pdstrategy_ = 0; }

   inline virtual int AreYouA(const ClassIdent &id) const;

   //! Can a plug be created on the given side?
   virtual bool canCreate(int side) const = 0;

   /**
    * Attempts to create a plug on the given side.
    *
    * When writing a derived class, your MakePlug function should \b ALWAYS
    * call CanCreate first before calling i_MakePlug.
    *
    * Returns 0 (NULL) on failure.  Only fails if CanCreate would return
    * false.
    */
   inline Plug *makePlug(int side);

   //! Does the module own this plug?
   virtual bool ownsPlug(const Plug *plug) const = 0;

   /**
    * Please delete this plug.
    *
    * Modules are supposed to own plugs, so you aren't supposed to delete them
    * yourself.
    *
    * Returns false on failure.  Failure could happen because module does not
    * own the plug.
    */
   virtual bool deletePlug(Plug *plug) = 0;

   /**
    * Get the current PlugDisconnectStrategy.
    *
    * Deleting the object you get this way is a BAD thing unless you set a new
    * strategy before you do it.
    */
   inline PlugDisconnectStrategy *getPDStrategy() const;
   /**
    * Set the current PlugDisconnectStrategy.
    *
    * The StreamModule does not assume responsibility for deleting the object
    * you pass in.  Of course, if you delete it while the StreamModule has it
    * set, that will probably cause bad things to happen.
    *
    * Setting the strategy to NULL essentially turns it off.  */
   inline void setPDStrategy(PlugDisconnectStrategy *pds);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   /**
    * Called whenever a plug is disconnected.
    *
    * <b>This function isn't called when a plug is disconnected as a result of
    * its destructor being called.</b>
    *
    * This is used to provide a hook to do what you think you should do when a
    * plug is disconnected.
    *
    * It calls the pdstrategy_ by default, if there is one.
    */
   inline virtual void plugDisconnected(Plug *plug);

   /**
    * Makes a plug on the given side.
    *
    * Guaranteed to never be called if canCreate would return false.  Must
    * NEVER return 0 (NULL).
    */
   virtual Plug *i_MakePlug(int side) = 0;

   //! Used to set the readable flag of a plug since their set method is
   //! protected.
   inline void setReadableFlagFor(Plug *p, bool val);
   //! Used to set the writeable flag of a plug since their set method is
   //! protected.
   inline void setWriteableFlagFor(Plug *p, bool val);

 private:
   PlugDisconnectStrategy *pdstrategy_;
};

/** \class StreamModule::Plug StreamModule.h StrMod/StreamModule.h
 * A point of connection between one StreamModule and another.
 */
class StreamModule::Plug : public Protocol {
 public:
   friend class StreamModule;
   static const STR_ClassIdent identifier;

   //! A Plug has to have a parent.
   inline Plug(StreamModule &parent);
   //! Does some tricky things to avoid having the disconnect strategy called.
   inline virtual ~Plug();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //! Can this plug be read from?
   bool isReadable() const   { return(flags_.canread_ && !flags_.isreading_); }
   //! Can this plug be written to?
   bool isWriteable() const  { return(flags_.canwrite_ && !flags_.iswriting_); }

   /**
    * \brief Plug this plug into another.  Can fail if already plugged in.
    */
   inline bool plugInto(Plug &other);
   //! Unplug this plug from any plugs it may be connected to.
   inline void unPlug();
   //! \brief Which plug (if any) is this plug plugged into?  Returns NULL if
   //! not plugged in.
   inline Plug *pluggedInto() const;

   //! Which module owns this plug?
   StreamModule &getParent() const            { return(parent_); }

   //! Which 'side' is this plug on?
   virtual int side() const = 0;

 protected:
   /**
    * A struct just so raw bitfields don't have to be in the class.
    */
   struct Flags {
       bool isreading_ : 1; ///< re-entrancy control
       bool iswriting_ : 1; ///< re-entrancy control
       bool canread_   : 1; ///< Can the plug be read from?
       bool canwrite_  : 1; ///< Can the plug be written to?
       bool notifyonread_  : 1; ///< Does the plug need to be told if its partner can be read from?
       bool notifyonwrite_ : 1; ///< Does the plug need to be told if its partner can be written to?
   };

   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   /**
    * Set whether this plug is readable or not.
    *
    * Even if this is set, isReadable can return false because the plug is
    * already being read from, and a plug can't be read by two things at once.
    */
   inline void setReadable(bool val);
   /**
    * Set whether this plug is writeable or not.
    *
    * Even if this is set, isWriteable can return false because the plug is
    * already being written to, and a plug can't be written to by two things
    * at once.
    */
   inline void setWriteable(bool val);

   //! These are so derived classes have access to this flag.
   void setIsReading(bool val)                { flags_.isreading_ = val; }
   //! These are so derived classes have access to this flag on other plugs.
   static void setIsReading(Plug &othr, bool v)        { othr.setIsReading(v); }
   //! These are so derived classes have access to this flag.
   void setIsWriting(bool val)                { flags_.iswriting_ = val; }
   //! These are so derived classes have access to this flag on other plugs.
   static void setIsWriting(Plug &othr, bool v)        { othr.setIsWriting(v); }

   //! This is so derived classes can get read access to the flags of any plug.
   inline static const Flags &getFlagsFrom(const Plug &p);

   /**
    * Tell a connected plug that this plug's readable state has changed.
    *
    * This checks the flags first and doesn't notify if the other plug doesn't
    * request notification.
    */
   inline void notifyOtherReadable() const;
   /**
    * Tell a connected plug that this plug's readable state has changed.
    *
    * This checks the flags first and doesn't notify if the other plug doesn't
    * request notification.
    */
   inline void notifyOtherWriteable() const;

   /**
    * If this plug needs to be notified of the other's readable state.
    *
    * The only plugs that should really need this are ones for modules where
    * the readable or writeable state of certain plugs depends on the readable
    * state of the plug that this plug is plugged into.
    */
   virtual bool needsNotifyReadable() const            { return(false); }
   /**
    * If this plug needs to be notified of the other's writeable state.
    *
    * The only plugs that should really need this are ones for modules where
    * the readable or writeable state of certain plugs depends on the
    * writeable state of the plug that this plug is plugged into.
    */
   virtual bool needsNotifyWriteable() const           { return(false); }

   /**
    * The 'other plug's readable state changed'  notification function.
    *
    * Called when plug this plug is plugged into changes whether or not it is
    * readable.
    */
   virtual void otherIsReadable()                      { }
   /**
    * The 'other plug's writeable state changed' notification function.
    *
    * Called when plug this plug is plugged into changes whether or not it is
    * writeable.
    */
   virtual void otherIsWriteable()                     { }

   /** @name Read and Write
    * Actually read or write to a StreamModule::Plug.
    *
    * These functions have undefined behavior when the appropriate isReadable
    * or isWriteable function returns false.
    *
    * Also, to avoid re-entrancy problems, the flag_.isreading_ or
    * flag_.iswriting_ flag should be set before actually doing the read or
    * write.  This prevents the read or write from causing the function to be
    * called again.
    *
    * This is of particular importance for modules like EchoModule.  If you
    * have two EchoModule objects with a buffering module of some sort between
    * then, an infinitely recurisve loop could easily be formed if a read or
    * write function were called without the appropriate re-entrancy control
    * flag being set.
    *
    * As it is, you will get an infinite loop, but it will
    * not be recursive, so the program will not crash.  (You might have wanted
    * the infinite loop for some reason.)
    */
   //@{
   /**
    * \brief Read this this plug.  Behavior is undefined if this plug not
    * readable.
    *
    * When writing your own read function that calls this one, you are
    * responsible for making sure the the plug is readable before calling this
    * function.
    *
    * These functions do not set the isreading or iswriting
    * flags.  You'll need to write wrapper functions that do this.  The
    * pushLoop and pullLoop functions already set the isreading and iswriting
    * flags.
    */
   virtual const StrChunkPtr i_Read() = 0;
   /**
    * \brief Write to this plug.  Behavior is undefined if this plug not
    * writeable.
    *
    * When writing your own write function that calls this one, you are
    * responsible for making sure the the plug is writeable before calling
    * this function.
    * 
    * These functions do not set the isreading or iswriting flags.  You'll
    * need to write wrapper functions that do this.  The pushLoop and pullLoop
    * functions already set the isreading and iswriting flags.
    */
   virtual void i_Write(const StrChunkPtr &ptr) = 0;
   //@}

   //! This is so derived classes can call write on the connected plug.
   void writeOther(const StrChunkPtr &ptr) const   { other_->i_Write(ptr); }
   //! This is so derived classes can call read on the connected plug.
   const StrChunkPtr readOther() const             { return(other_->i_Read()); }

   /**
    * Read as much as possible from this plug and write to the connected plug.
    */
   void pushLoop();
   /**
    * Read as much as possible from the connected plug and write to this plug.
    */
   void pullLoop();

 private:
   Flags flags_;  ///< See struct Flags.
   Plug *other_;  ///< The plug I'm talking to (if any).
   StreamModule &parent_;  ///< What StreamModule I came from.
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

};  // namespace strmod
};  // namespace strmod

#endif
