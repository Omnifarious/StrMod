#ifndef _STR_StreambufModule_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.2  1995/07/23 04:04:19  hopper
// Changed things for integration into the rest of my libraries.
//
// Revision 1.1.1.1  1995/07/22 04:46:51  hopper
// Imported sources
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
#   ifndef _STR_StreamModule_H_
#      include "strmod.h"
#   endif
#else
#   ifndef _STR_StreamModule_H_
#      include <StrMod/StreamModule.h>
#   endif
#endif

#include <assert.h>

#define  _STR_StreambufModule_H_

#ifndef NULLREPLACE
#   define NULLREPLACE ('\01')
#endif

class StrbufPlug;
class StrChunk;
class istrstream;
class ostrstream;

//----class StreambufModule
// This is a very stupid, application specific StreamModule, and I hope
// nobody uses it.

class StreambufModule : public StreamModule {
friend class StrbufPlug;

 public:
   static const STR_ClassIdent identifier;

   inline StreambufModule(char nrepl = NULLREPLACE);
   virtual ~StreambufModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanCreate(int side) const;
   StrbufPlug *MakePlug(int side);
   inline virtual bool OwnsPlug(StrPlug *plug) const;
   virtual bool DeletePlug(StrPlug *plug);

   istrstream *GetStream(StrChunk *&bufspace); //** bufspace should be deleted
   inline bool CanGet();                       //   when done with istrstream.

   bool PutStream(ostrstream *os);  // **Note: delete's os
   bool PutStream(ostrstream &os);
   inline bool CanPut();

   void SetNullReplace(char newrepl)     { nullreplace = newrepl; }
   char GetNullReplace() const           { return(nullreplace); }

 protected:
   StrbufPlug *splug;
   int plugcreated;
   int rdngfrm, wrtngto;
   char nullreplace;

   inline virtual StrPlug *CreatePlug(int side);

   virtual void CanBeGotten()            { }
};

//----class StrbufPlug

class StrbufPlug : public StrPlug {
friend class StreambufModule;

 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanWrite() const;
   inline virtual bool Write(StrChunk *);

   inline virtual bool CanRead() const;

   inline StreambufModule *ModuleFrom() const;
   virtual int Side() const                          { return(0); }

 protected:
   StreambufModule *parent;

   inline StrbufPlug(StreambufModule *parnt);
   inline virtual ~StrbufPlug();

   virtual StreamModule *ParentModule() const        { return(parent); }
   virtual StrChunk *InternalRead()                  { return(0); }

   inline virtual void ReadableNotify();
   inline virtual void WriteableNotify();
};

//-----------------------------inline functions--------------------------------

inline StreambufModule::StreambufModule(char nrepl)
   : splug(0), plugcreated(0), rdngfrm(0), wrtngto(0), nullreplace(nrepl)
{
   splug = new StrbufPlug(this);
}

inline int StreambufModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool StreambufModule::CanCreate(int side) const
{
   return(side == 0 && !plugcreated);
}

inline StrbufPlug *StreambufModule::MakePlug(int side)
{
   return((StrbufPlug *)(CreatePlug(side)));
}

inline bool StreambufModule::OwnsPlug(StrPlug *plug) const
{
   return(plugcreated && (plug == splug));
}

inline bool StreambufModule::CanGet()
{
   StrPlug *oplug;

   if (plugcreated && (oplug = splug->PluggedInto()))
      return(oplug->CanRead());
   else
      return(false);
}

inline bool StreambufModule::CanPut()
{
   StrPlug *oplug;

   if (plugcreated && (oplug = splug->PluggedInto()))
      return(oplug->CanWrite());
   else
      return(false);
}

inline StrPlug *StreambufModule::CreatePlug(int side)
{
   if (side == 0 && !plugcreated) {
      plugcreated = 1;
      return(splug);
   } else
      return(0);
}

//=========StrbufPlug inlines========

inline int StrbufPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrPlug::AreYouA(cid));
}

inline bool StrbufPlug::CanWrite() const
{
   return(false);
}

inline bool StrbufPlug::Write(StrChunk *)
{
   return(false);
}

inline bool StrbufPlug::CanRead() const
{
   return(false);
}

inline StreambufModule *StrbufPlug::ModuleFrom() const
{
   return(static_cast<StreambufModule *>(ParentModule()));
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

inline void StrbufPlug::ReadableNotify()
{
   ModuleFrom()->CanBeGotten();
}

inline void StrbufPlug::WriteableNotify()
{
   return;
}

#endif
