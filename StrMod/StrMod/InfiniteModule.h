#ifndef _STR_InfiniteModule_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/StreamModule.h>
#include <StrMod/StrChunkPtr.h>

#define _STR_InfiniteModule_H_

//: Sends a particular chunk repeatedly forever.  Eats any chunk sent to it.
// <p>The strongly resembles a /dev/zero where you decide what comes out
// instead of an infinite string of zeros.</p>
class InfiniteModule : public StreamModule {
   class IPlug;
   friend class IPlug;

 public:
   static const STR_ClassIdent identifier;

   //: Repeatedly sends chnk.
   InfiniteModule(const StrChunkPtr &chnk);
   //: Defaults to a PreAllocBuffer containing 8192 zeros.
   InfiniteModule();
   virtual ~InfiniteModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool canCreate(int side = 0) const;
   inline Plug *makePlug(int side = 0);
   inline virtual bool ownsPlug(const Plug *plug) const;
   virtual bool deletePlug(Plug *plug);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual Plug *i_MakePlug(int side);

 private:
   class IPlug : public Plug {
    public:
      friend class InfiniteModule;

      static const STR_ClassIdent identifier;

      IPlug(InfiniteModule &parent) : Plug(parent)      { }
      ~IPlug()                                          { }

      inline virtual int AreYouA(const ClassIdent &cid) const;

      inline InfiniteModule &getParent() const;
      virtual int side() const                          { return(0); }

    protected:
      virtual const ClassIdent *i_GetIdent() const      { return(&identifier); }

      inline virtual const StrChunkPtr i_Read();
      inline virtual void i_Write(const StrChunkPtr &tr);
   };

   StrChunkPtr feed_;
   bool plug_created_;
   IPlug plug_;
};

//-----------------------------inline functions--------------------------------

inline int InfiniteModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool InfiniteModule::canCreate(int side) const
{
   return((side == 0) && !plug_created_);
}

inline StreamModule::Plug *InfiniteModule::makePlug(int side)
{
   return(StreamModule::makePlug(side));
}

inline bool InfiniteModule::ownsPlug(const Plug *plug) const
{
   return((&plug_ == plug) && plug_created_);
}

//--

inline int InfiniteModule::IPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Plug::AreYouA(cid));
}

inline InfiniteModule &InfiniteModule::IPlug::getParent() const
{
   return(static_cast<InfiniteModule &>(Plug::getParent()));
}

const StrChunkPtr InfiniteModule::IPlug::i_Read()
{
   return(getParent().feed_);
}

void InfiniteModule::IPlug::i_Write(const StrChunkPtr &ptr)
{
}

#endif
