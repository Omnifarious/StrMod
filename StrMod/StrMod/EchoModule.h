#ifndef _STR_EchoModule_H_  // -*-c++-*-

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// For log information, see ../ChangeLog

// $Revision$

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif

#include <assert.h>

#define _STR_EchoModule_H_

class StrChunk;

//: This module echoes everything that comes in.
// <p>Everything that is written to its one plug is read from that same
// plug.</p>
// <p>See the parent class for any member functions not described here.</p>
class EchoModule : public StreamModule {
   class EPlug;
   friend class EPlug;
 public:
   static const STR_ClassIdent identifier;

   EchoModule();
   virtual ~EchoModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   /*lint -save -e1735 */
   inline virtual bool canCreate(int side = 0) const;
   /*lint -e1511 */
   inline Plug *makePlug(int side = 0);
   /*lint -restore */
   virtual bool ownsPlug(const Plug *plug) const   { return(i_OwnsPlug(plug)); }
   virtual bool deletePlug(Plug *plug);

 protected:
   virtual const ClassIdent *i_GetIdent() const    { return(&identifier); }

   virtual void plugDisconnected(Plug *plug);

   inline virtual Plug *i_MakePlug(int side);

 private:
   //: This is the magical plug that does most of EchoModule's work.
   // <p>See the parent class for descriptions of these member functions.</p>
   class EPlug : public Plug {
      friend class EchoModule;
    public:
      static const STR_ClassIdent identifier;

      inline virtual int AreYouA(const ClassIdent &cid) const;

      inline EchoModule &getParent() const;

      virtual int side() const                          { return(0); }

    protected:
      inline EPlug(EchoModule &parnt);
      inline virtual ~EPlug();

      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      virtual bool needsNotifyReadable() const          { return(true); }
      virtual bool needsNotifyWriteable() const         { return(true); }

      virtual void otherIsReadable();
      virtual void otherIsWriteable();

      virtual const StrChunkPtr i_Read();
      virtual void i_Write(const StrChunkPtr &ptr);
   };

   bool plugcreated_;
   EPlug eplug_;

   inline bool i_OwnsPlug(const Plug *plug) const;
};

//-------------------------------inline functions------------------------------

inline int EchoModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

bool EchoModule::canCreate(int side) const
{
   return(side == 0 && !plugcreated_);
}

inline StreamModule::Plug *EchoModule::makePlug(int side)
{
   return(StreamModule::makePlug(side));
}

inline bool EchoModule::i_OwnsPlug(const Plug *plug) const
{
   return(plugcreated_ && (plug == &eplug_));
}

inline EchoModule::Plug *EchoModule::i_MakePlug(int side)
{
   assert(side == 0 && !plugcreated_);
   plugcreated_ = true;
   //lint -save -e1536
   return(&eplug_);
   //lint -restore
}

//=========EchoModule::EPlug inlines========

inline EchoModule::EPlug::EPlug(EchoModule &parnt) : Plug(parnt)
{
}

inline EchoModule::EPlug::~EPlug()
{
}

inline int EchoModule::EPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline EchoModule &EchoModule::EPlug::getParent() const
{
   return(static_cast<EchoModule &>(Plug::getParent()));
}

#endif
