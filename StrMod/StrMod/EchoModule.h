#ifndef _STR_EchoModule_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
// Revision 0.8  1995/04/05  04:51:39  hopper
// Fixed a few stupid mistakes.
//
// Revision 0.7  1995/04/05  01:49:00  hopper
// Changed things for integration into the rest of my libraries.
//
// Revision 0.6  1994/06/16  02:43:10  hopper
// Added #pragma interface declaration.
//
// Revision 0.5  1994/06/13  13:24:36  hopper
// Changed EchoModule::MakePlug(int side) to return an EchoPlug *, like it's
// supposed to, not a StrPlug *.
//
// Revision 0.4  1994/06/12  04:42:09  hopper
// Made major changes so EchoModule would work.
//
// Revision 0.3  1994/06/10  14:13:05  hopper
// Fixed a syntax error, and made changes so it would work better under OS2.
//
// Revision 0.2  1994/06/08  17:27:22  hopper
// Fixed lots of stupid errors.
// Moved EchoModule::DeletePlug to EchoModule.cc because it became too
// complicated to be inline.
//
// Revision 0.1  1994/06/08  17:05:32  hopper
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

#define _STR_EchoModule_H_

class EchoPlug;
class StrChunk;

//----class EchoModule

class EchoModule : public StreamModule {
friend class EchoPlug;

 public:
   static const STR_ClassIdent identifier;

   inline EchoModule();
   virtual ~EchoModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanCreate(int side = 0) const;
   inline EchoPlug *MakePlug(int side = 0);
   inline virtual bool OwnsPlug(StrPlug *plug) const;
   virtual bool DeletePlug(StrPlug *plug);

 protected:
   StrChunk *buffedecho;
   EchoPlug *eplug;
   int plugcreated;
   int rdngfrm, wrtngto;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual StrPlug *CreatePlug(int side);
};

//----class EchoPlug

class EchoPlug : public StrPlug {
friend class EchoModule;

 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanWrite() const;
   virtual bool Write(StrChunk *);

   virtual bool CanRead() const;

   inline EchoModule *ModuleFrom() const;
   virtual int Side() const                            { return(0); }

 protected:
   EchoModule *parent;

   inline EchoPlug(EchoModule *parnt);
   inline virtual ~EchoPlug();

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual StreamModule *ParentModule() const          { return(parent); }
   virtual StrChunk *InternalRead();

   virtual void ReadableNotify();
   virtual void WriteableNotify();

};

//-------------------------------inline functions------------------------------

inline EchoModule::EchoModule()
  : buffedecho(0), eplug(0), plugcreated(0), rdngfrm(0), wrtngto(0)
{
   eplug = new EchoPlug(this);
}
   
inline int EchoModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

bool EchoModule::CanCreate(int side) const
{
   return(side == 0 && !plugcreated);
}

inline EchoPlug *EchoModule::MakePlug(int side)
{
   return((EchoPlug *)(CreatePlug(side)));
}

inline bool EchoModule::OwnsPlug(StrPlug *plug) const
{
   return(plugcreated && (plug == eplug));
}

inline StrPlug *EchoModule::CreatePlug(int side)
{
   if (side == 0 && !plugcreated) {
      plugcreated = 1;
      return(eplug);
   } else
      return(0);
}

//=========EchoPlug inlines========

inline int EchoPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrPlug::AreYouA(cid));
}

inline bool EchoPlug::CanWrite() const
{
   EchoModule *parent = ModuleFrom();

   return(parent->plugcreated &&
	  !(parent->wrtngto) && (parent->buffedecho == 0));
}

inline bool EchoPlug::CanRead() const
{
   EchoModule *parent = ModuleFrom();

   return(parent->plugcreated &&
	  !(parent->rdngfrm) && (parent->buffedecho != 0));
}

inline EchoModule *EchoPlug::ModuleFrom() const
{
   return((EchoModule *)(ParentModule()));
}

inline EchoPlug::EchoPlug(EchoModule *parnt)
  : parent(parnt)
{
   assert(parent != 0);
}

inline EchoPlug::~EchoPlug()
{
   if (PluggedInto())
      PluggedInto()->UnPlug();
}

#endif
