#ifndef _STR_SimpleMulti_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log, see ../ChangeLog

#include <StrMod/StreamModule.h>
#include <StrMod/SimplePlug.h>
#include <list>

#define _STR_SimpleMulti_H_

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
   class SinglePlug;
   friend class MultiPlug;
   friend class SinglePlug;

 public:
   enum PublicSides { SingleSide, MultiSide };
   static const STR_ClassIdent identifier;

   SimpleMultiplexer();
   virtual ~SimpleMultiplexer();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanCreate(int side) const;
   virtual bool OwnsPlug(StrPlug *plug) const;
   virtual bool DeletePlug(StrPlug *plug);

 protected:
   class SinglePlug : public SimplePlugOf<SimpleMultiplexer> {
      friend class SimpleMultiplexer;
    public:
      static const STR_ClassIdent identifier;

      inline virtual int AreYouA(const ClassIdent &cid) const;

      virtual int Side() const                         { return(SingleSide); }

    protected:
      inline SinglePlug(SimpleMultiplexer *parent);
      inline virtual ~SinglePlug();

      virtual const ClassIdent *i_GetIdent() const     { return(&identifier); }

      inline virtual bool i_CanWrite() const;
      inline virtual bool i_Write(const StrChunkPtr &);
      inline virtual bool i_CanRead() const;
      inline virtual const StrChunkPtr i_InternalRead();
   };

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }
   virtual StrPlug *CreatePlug(int side);

   bool SingleCanWrite() const                         { return(!mchunk_); }
   bool SingleWrite(const StrChunkPtr &chnk);
   bool SingleCanRead();
   const StrChunkPtr SingleRead();

   inline bool MultiCanWrite() const;
   bool MultiWrite(const StrChunkPtr &ptr);
   inline bool MultiCanRead(const MultiPlug *mplug) const;
   const StrChunkPtr MultiRead(MultiPlug *mplug);

 private:
   typedef list<MultiPlug *> MPlugList;

   SinglePlug splug_;
   bool splug_created_;
   MPlugList mplugs_;
   MPlugList::iterator nextplug_;
   StrChunkPtr mchunk_;
   unsigned int multis_left_to_read_mchunk_;
};

//--

class SimpleMultiplexer::MultiPlug : public SimplePlugOf<SimpleMultiplexer> {
   friend class SimpleMultiplexer;
 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual int Side() const                            { return(MultiSide); }

 protected:
   inline MultiPlug(SimpleMultiplexer *parent);
   inline virtual ~MultiPlug();

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual bool i_CanWrite() const;
   inline virtual bool i_Write(const StrChunkPtr &);
   inline virtual bool i_CanRead() const;
   inline virtual const StrChunkPtr i_InternalRead();

   bool HasRead() const                                { return(hasread_); }
   void ClearHasRead()                                 { hasread_ = false; }
   void SetHasRead()                                   { hasread_ = true; }

 private:
   bool hasread_;
};

//-----------------------------inline functions--------------------------------

inline int SimpleMultiplexer::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool SimpleMultiplexer::CanCreate(int side) const
{
   if (side == SingleSide) {
      return(!splug_created_);
   } else if (side == MultiSide) {
      return(true);
   } else {
      return(false);
   }
}

inline bool SimpleMultiplexer::MultiCanWrite() const
{
   return(splug_created_ && splug_.PluggedInto() && !splug_.IsReading() &&
	  splug_.PluggedInto()->CanWrite());
}

inline bool SimpleMultiplexer::MultiCanRead(const MultiPlug *mplug) const
{
   return(!mplug->HasRead() && mchunk_);
}

//--

inline SimpleMultiplexer::SinglePlug::SinglePlug(SimpleMultiplexer *parent)
     : SimplePlugOf<SimpleMultiplexer>(parent)
{
}

inline SimpleMultiplexer::SinglePlug::~SinglePlug()
{
   UnPlug();
}

inline int SimpleMultiplexer::SinglePlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) ||
	  SimplePlugOf<SimpleMultiplexer>::AreYouA(cid));
}

bool SimpleMultiplexer::SinglePlug::i_CanWrite() const
{
   return(ModuleFrom()->SingleCanWrite());
}

inline bool SimpleMultiplexer::SinglePlug::i_Write(const StrChunkPtr &chnk)
{
   return(ModuleFrom()->SingleWrite(chnk));
}

bool SimpleMultiplexer::SinglePlug::i_CanRead() const
{
   return(ModuleFrom()->SingleCanRead());
}

inline const StrChunkPtr SimpleMultiplexer::SinglePlug::i_InternalRead()
{
   return(ModuleFrom()->SingleRead());
}
   

//--

inline SimpleMultiplexer::MultiPlug::MultiPlug(SimpleMultiplexer *parent)
     : SimplePlugOf<SimpleMultiplexer>(parent), hasread_(false)
{
}

inline SimpleMultiplexer::MultiPlug::~MultiPlug()
{
   UnPlug();
}

inline int SimpleMultiplexer::MultiPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) ||
	  SimplePlugOf<SimpleMultiplexer>::AreYouA(cid));
}

inline bool SimpleMultiplexer::MultiPlug::i_CanWrite() const
{
   return(ModuleFrom()->MultiCanWrite());
}

inline bool SimpleMultiplexer::MultiPlug::i_Write(const StrChunkPtr &ptr)
{
   return(ModuleFrom()->MultiWrite(ptr));
}

inline bool SimpleMultiplexer::MultiPlug::i_CanRead() const
{
  return(ModuleFrom()->MultiCanRead(this));
}

inline const StrChunkPtr SimpleMultiplexer::MultiPlug::i_InternalRead()
{
   return(ModuleFrom()->MultiRead(this));
}

#endif
