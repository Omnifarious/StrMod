#ifndef _EH_StreambufModule_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
// Revision 0.4  1995/01/05  21:51:51  hopper
// Added return statements in rarely taken if branches, These would've caused
// serious problems if they'd been triggered.
//
// Revision 0.3  1994/06/16  14:03:33  hopper
// Fixed various errors and barriers to compilation.
//
// Revision 0.2  1994/06/16  03:48:38  hopper
// Added inline function StrbufPlug::Write(StrChunk *).
//
// Revision 0.1  1994/06/16  02:46:28  hopper
// Genesis!
//

// $Revision$

#ifdef OS2
#   ifndef _EH_StreamModule_H_
#      include "strmod.h"
#   endif
#else
#   ifndef _EH_StreamModule_H_
#      include <StrMod/StreamModule.h>
#   endif
#endif

#include <assert.h>

#define  _EH_StreambufModule_H_

#ifndef NULLREPLACE
#   define NULLREPLACE ('\01')
#endif

class StrbufPlug;
class StrChunk;
class istrstream;
class ostrstream;

//----class StreambufModule

class StreambufModule : public StreamModule {
friend class StrbufPlug;

 protected:
   StrbufPlug *splug;
   int plugcreated;
   int rdngfrm, wrtngto;
   char nullreplace;

   inline virtual StrPlug *CreatePlug(int side);

   virtual void CanBeGotten()            { }

 public:
   virtual int CanCreate(int side) const { return(side == 0 && !plugcreated); }
   StrbufPlug *MakePlug(int side);
   inline virtual int OwnsPlug(StrPlug *plug) const;
   virtual int DeletePlug(StrPlug *plug);

   virtual int ModuleType() const        { return(StreambufModuleNum); }
   inline virtual int AreYouA(int mtype) const;

   istrstream *GetStream(StrChunk *&bufspace); //** bufspace should be deleted
   inline int CanGet();                        //   when done with istrstream.

   int PutStream(ostrstream *os);  // **Note: delete's os
   int PutStream(ostrstream &os);
   inline int CanPut();

   void SetNullReplace(char newrepl)     { nullreplace = newrepl; }
   char GetNullReplace() const           { return(nullreplace); }

   inline StreambufModule(char nrepl = NULLREPLACE);
   virtual ~StreambufModule();
};

//----class StrbufPlug

class StrbufPlug : public StrPlug {
friend class StreambufModule;

 protected:
   StreambufModule *parent;

   virtual StreamModule *ParentModule() const        { return(parent); }
   virtual StrChunk *InternalRead()                  { return(0); }

   inline virtual void ReadableNotify();
   inline virtual void WriteableNotify();

   inline StrbufPlug(StreambufModule *parnt);
   inline virtual ~StrbufPlug();

 public:
   virtual int PlugType() const                      { return(StrbufPlugNum); }
   inline virtual int AreYouA(int ptype) const;

   inline virtual int CanWrite() const;
   inline virtual int Write(StrChunk *);

   inline virtual int CanRead() const;

   inline StreambufModule *ModuleFrom() const;
   virtual int Side() const                          { return(0); }
};

//-----------------------------inline functions--------------------------------

inline StrPlug *StreambufModule::CreatePlug(int side)
{
   if (side == 0 && !plugcreated) {
      plugcreated = 1;
      return(splug);
   } else
      return(0);
}

inline StrbufPlug *StreambufModule::MakePlug(int side)
{
   return((StrbufPlug *)(CreatePlug(side)));
}

inline int StreambufModule::OwnsPlug(StrPlug *plug) const
{
   return(plugcreated && (plug == splug));
}

inline int StreambufModule::AreYouA(int mtype) const
{
   return((mtype == StreambufModuleNum) || StreamModule::AreYouA(mtype));
}

inline int StreambufModule::CanGet()
{
   StrPlug *oplug;

   if (plugcreated && (oplug = splug->PluggedInto()))
      return(oplug->CanRead());
   else
      return(0);
}

inline int StreambufModule::CanPut()
{
   StrPlug *oplug;

   if (plugcreated && (oplug = splug->PluggedInto()))
      return(oplug->CanWrite());
   else
      return(0);
}

inline StreambufModule::StreambufModule(char nrepl)
   : splug(0), plugcreated(0), rdngfrm(0), wrtngto(0), nullreplace(nrepl)
{
   splug = new StrbufPlug(this);
}

//=========StrbufPlug inlines========

inline void StrbufPlug::ReadableNotify()
{
   ModuleFrom()->CanBeGotten();
}

inline void StrbufPlug::WriteableNotify()
{
   return;
}

inline int StrbufPlug::AreYouA(int ptype) const
{
   return((ptype == StrbufPlugNum) || StrPlug::AreYouA(ptype));
}

inline int StrbufPlug::CanWrite() const
{
   return(0);
}

inline int StrbufPlug::Write(StrChunk *)
{
   return(0);
}

inline int StrbufPlug::CanRead() const
{
   return(0);
}

inline StreambufModule *StrbufPlug::ModuleFrom() const
{
   return((StreambufModule *)(ParentModule()));
}

inline StrbufPlug::StrbufPlug(StreambufModule *parnt)
  : parent(parnt)
{
   assert(parent != 0);
}

inline StrbufPlug::~StrbufPlug()
{
   if (PluggedInto())
      UnPlug();
}

#endif
