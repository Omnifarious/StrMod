#ifndef _EH_StreamSplitter_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/22 04:46:50  hopper
 // Initial revision
 //
 // -> Revision 0.13  1995/04/14  15:33:37  hopper
 // -> Combined revision 0.12, and 0.12.0.3
 // ->
 // -> Revision 0.12.0.3  1995/04/14  15:28:41  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.12.0.2  1994/06/16  02:16:22  hopper
 // -> Added #pragma interface declaration.
 // ->
 // -> Revision 0.12.0.1  1994/05/16  04:37:08  hopper
 // -> No changes, but I needed a head for the inevitable WinterFire-OS/2
 // -> branch.
 // ->
 // -> Revision 0.12  1994/05/07  03:40:55  hopper
 // -> Move enclosing #ifndef ... #define bits around. Changed names of some
 // -> non-multiple include constants. Changed directories for certain
 // -> type of include files.
 // ->
 // -> Revision 0.11  1992/04/27  22:39:01  hopper
 // -> Added inline definition for
 // -> SplitterPlug::SplitterPlug(SplitterModule *, int)
 // ->
 // -> Revision 0.10  1992/04/27  21:31:14  hopper
 // -> Genesis!
 // ->

#ifndef NO_RcsID
static char _EH_StreamSplitter_H_rcsID[] =
      "$Id$";
#endif

#ifndef _EH_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#define _EH_StreamSplitter_H_

// SplitterModule's work this way
//
//  Module----->--->-->--+
//                       |       (bi-directional)
//            side 0->  Splitter===>==<==>==<==>==<===Module
//                       |      ^
//  Module-----<---<--<--+      |
//                              +--side 1
//
// The SplitterModule takes three i/o streams, one that does both input and
// output, one that does input, and one that does output, and connects them
// together. They're connected in such a way as to cause the output of the
// stream that does input and ouput goes into the stream that only takes input,
// and the output of the stream that only does output goes to the input of the
// combined stream.
// The Splitter module coughs & dies if the ouput only module tries to read from
// it. If the input only module tries to write to it, the output goes into the
// bit-bucket.
// The same plug type is used for all the connections, but they probably should
// be different types. I just didn't want to clutter the name space unnecesarily
// with names. When nested classes become more reliable under g++ perhaps this
// will be implemented.


class SplitterPlug;

// All terms like inputonly, or outputonly are from the point of view of the
// SplitterModule, and not the modules the terms reference.

class SplitterModule : public StreamModule {
   friend class SplitterPlug;

 public:
   enum PlugTypes { CombinedPlug, InputOnlyPlug, OutputOnlyPlug };

   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanCreate(int side) const;
   inline bool CanCreate(PlugTypes plugtype) const;
   inline SplitterPlug *MakePlug(int plugtype);
   inline SplitterPlug *MakePlug(PlugTypes plugtype);
   inline virtual bool OwnsPlug(StrPlug *plug) const;
   virtual bool DeletePlug(StrPlug *plug);

   SplitterModule();
   virtual ~SplitterModule();

 protected:
   SplitterPlug *combined, *inputonly, *outputonly;
   struct {
      unsigned int combined   : 1;
      unsigned int inputonly  : 1;
      unsigned int outputonly : 1;
   } created;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual StrPlug *CreatePlug(int side);
};

//----------------------------class SplitterPlug-------------------------------

class StrChunk;

class SplitterPlug : public StrPlug {
   friend class SplitterModule;

 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual SplitterModule::PlugTypes PlugType() const  { return(sp_plugtype); }

   virtual bool CanWrite() const;
   virtual bool Write(StrChunk *);

   virtual bool CanRead() const;

   inline SplitterModule *ModuleFrom() const;
   inline virtual int Side() const;

 protected:
   inline SplitterPlug(SplitterModule *parent,
		       SplitterModule::PlugTypes ptype);
   ~SplitterPlug()                                     { }

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual StreamModule *ParentModule() const          { return(smod); }
   virtual StrChunk *InternalRead();

   virtual void ReadableNotify();
   virtual void WriteableNotify();

 private:
   SplitterModule *smod;
   SplitterModule::PlugTypes sp_plugtype;
};

//--------------------------SplitterModule inlines-----------------------------

inline int SplitterModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool SplitterModule::CanCreate(int side) const
{
   switch (side) {
    case CombinedPlug:
      return(combined && !created.combined);
      break;
    case InputOnlyPlug:
      return(inputonly && !created.inputonly);
      break;
    case OutputOnlyPlug:
      return(outputonly && !created.outputonly);
      break;
    default:
      return(false);
   }
}

inline bool SplitterModule::CanCreate(PlugTypes side) const
{
   return(CanCreate(int(side)));
}

inline SplitterPlug *SplitterModule::MakePlug(int plugtype)
{
   if (plugtype >= CombinedPlug && plugtype <= OutputOnlyPlug)
      return((SplitterPlug *)(CreatePlug(plugtype)));
   else
      return(0);
}

inline SplitterPlug *SplitterModule::MakePlug(PlugTypes plugtype)
{
   return(MakePlug(int(plugtype)));
}

inline bool SplitterModule::OwnsPlug(StrPlug *plug) const
{
   return((plug == combined && created.combined) ||
	  (plug == inputonly && created.inputonly) ||
	  (plug == outputonly && created.outputonly));
}

//---------------------------SplitterPlug inlines------------------------------

inline int SplitterPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrPlug::AreYouA(cid));
}

inline SplitterModule *SplitterPlug::ModuleFrom() const
{
   return((SplitterModule *)ParentModule());
}

inline int SplitterPlug::Side() const
{
   return(sp_plugtype);
}

SplitterPlug::SplitterPlug(SplitterModule *parent,
			   SplitterModule::PlugTypes ptype)
  : smod(parent), sp_plugtype(ptype)
{
}

#endif
