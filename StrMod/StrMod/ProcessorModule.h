#ifndef _STR_ProcessorModule_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.2  1996/09/21 18:38:21  hopper
// Added forward declaration of class ProcessorModule::Plug, presumably to
// work around a compiler bug.
//
// Revision 1.1  1996/09/02 23:31:20  hopper
// Added class that would turn two StreamProcessors into a StreamModule
// that used one StreamProcessor for the data flowing in each direction.
//

#include <StrMod/STR_ClassIdent.h>
#include <StrMod/StreamModule.h>
#include <StrMod/StreamProcessor.h>
#include <StrMod/SimplePlug.h>

#define _STR_ProcessorModule_H_

class StrChunk;

class ProcessorModule : public StreamModule {
 public:
   static const STR_ClassIdent identifier;

   class Plug;
   enum PlugSide { OneSide, OtherSide };
   class Plug : public SimplePlugOf<ProcessorModule> {
      friend class ProcessorModule;

    public:
      typedef SimplePlugOf<ProcessorModule> parentclass;

      static const STR_ClassIdent identifier;

      inline virtual int AreYouA(const ClassIdent &cid) const;

      inline virtual int Side() const;

    protected:
      virtual const ClassIdent *i_GetIdent() const     { return(&identifier); }

      inline virtual bool i_CanWrite() const;
      virtual bool i_Write(const StrChunkPtr &);
      inline virtual bool i_CanRead() const;
      virtual const StrChunkPtr i_InternalRead();

   private:
      Plug &otherplug_;
      PlugSide side_;
      StreamProcessor * const inproc_;
      StreamProcessor * const outproc_;

      inline Plug(ProcessorModule *prnt, Plug &otherplug, PlugSide side,
		  StreamProcessor *inproc, StreamProcessor *outproc);
      virtual ~Plug()                                  { }

      void CheckNotify();
   };

   ProcessorModule(StreamProcessor *in, StreamProcessor *out);
   virtual ~ProcessorModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline bool CanCreate(PlugSide side) const;
   inline Plug *MakePlug(PlugSide side);
   inline virtual bool OwnsPlug(StrPlug *plug) const;
   virtual bool DeletePlug(StrPlug *plug);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual StrPlug *CreatePlug(int side);

   virtual bool CanCreate(int side) const;

 private:
   Plug side_, otherside_;
   struct {
      bool side : 1;
      bool otherside : 1;
   } pulled_;
};

//-----------------------------inline functions--------------------------------

//----------ProcessorModule::Plug inlines-----

inline int ProcessorModule::Plug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || parentclass::AreYouA(cid));
}

inline bool ProcessorModule::Plug::i_CanWrite() const
{
   return(inproc_->CanAddIncoming());
}

inline bool ProcessorModule::Plug::i_CanRead() const
{
   return(outproc_->CanPullOutgoing());
}

inline int ProcessorModule::Plug::Side() const
{
   return(side_);
}

inline ProcessorModule::Plug::Plug(ProcessorModule *prnt, Plug &other,
				   PlugSide side,
				   StreamProcessor *i, StreamProcessor *o)
     : parentclass(prnt), otherplug_(other),
       side_(side), inproc_(i), outproc_(o)
{
}

//----------ProcessorModule inlines-----

inline int ProcessorModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool ProcessorModule::CanCreate(PlugSide side) const
{
   return((side == OneSide) ? !pulled_.side : !pulled_.otherside);
}

inline ProcessorModule::Plug *ProcessorModule::MakePlug(PlugSide side)
{
   StrPlug *tmp;

   tmp = CreatePlug(side);
   return(static_cast<ProcessorModule::Plug *>(tmp));
}

inline bool ProcessorModule::OwnsPlug(StrPlug *plug) const
{
   const StrPlug * const sidep = &side_;
   const StrPlug * const othersidep = &otherside_;

   return((plug == sidep && pulled_.side) ||
	  (plug == othersidep && pulled_.otherside));
}

#endif
