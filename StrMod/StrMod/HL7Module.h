#ifndef _EH_HL7Module_H_

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:51  hopper
// Initial revision
//
// Revision 0.5  1995/01/13  17:09:16  hopper
// Changed the #define for the non-double include constant to be correct.
//
// Revision 0.4  1995/01/05  21:35:10  hopper
// Changed the state machine that matched message seperators into an
// implementation of the KMP (Knuth Morris Pratt) string matching algorithm.
//
// Revision 0.3  1994/06/21  16:01:51  hopper
// Fixed HL7Module::CreatePlug(int side) to work properly.
//
// Revision 0.2  1994/06/21  02:56:28  hopper
// Changed some things to be inline that should've been.
// Changed other things to not be inline.
// Added HL7Module::OwnsPlug(StrPlug *plug) const
// Changed HL7Module::ChangeState(int, char) to match \nEOM|\n instead of
// \nENDM\n
//
// Revision 0.1  1994/06/20  04:33:14  hopper
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

#define  _EH_HL7Module_H_

class HL7Plug;
class StrChunk;
class DataBlockStrChunk;

class HL7Module : public StreamModule {
friend class HL7Plug;

 protected:
   HL7Plug *plug0, *plug1;
   struct {
      unsigned int zero : 1;
      unsigned int one : 1;
   } created;
   DataBlockStrChunk *message, *processing;
   unsigned int messagedone;
   unsigned int bytesprocessed;
   unsigned short seplen, nexttest;
   unsigned short prefixtable[128];
   char seperator[128];

   inline virtual StrPlug *CreatePlug(int side);

   void DoProcessing();

 public:
   inline virtual int CanCreate(int side) const;
   inline HL7Plug *MakePlug(int side);
   inline virtual int OwnsPlug(StrPlug *plug) const;
   virtual int DeletePlug(StrPlug *plug);

   const char *GetSeperator() const      { return(seperator); }
   void SetSeperator(const char *s);

   virtual int ModuleType() const        { return(HL7ModuleNum); }
   inline virtual int AreYouA(int mtype) const;

   inline HL7Module();
   virtual ~HL7Module();
};

//----class HL7Plug

class HL7Plug : public StrPlug {
friend class HL7Module;

 protected:
   HL7Module *parent;
   int side;
   struct {
      unsigned int wrtngto : 1;
      unsigned int rdngfrm : 1;
   } ioflags;

   virtual StreamModule *ParentModule() const        { return(parent); }
   virtual StrChunk *InternalRead();

   virtual void ReadableNotify();
   virtual void WriteableNotify();

   inline HL7Plug(HL7Module *parnt, int side);
   inline virtual ~HL7Plug();

 public:
   virtual int PlugType() const                      { return(HL7PlugNum); }
   inline virtual int AreYouA(int ptype) const;

   virtual int CanWrite() const;
   virtual int Write(StrChunk *);

   virtual int CanRead() const;

   inline HL7Module *ModuleFrom() const;
   virtual int Side() const                          { return(side); }
};

//-----------------------------inline functions--------------------------------


inline StrPlug *HL7Module::CreatePlug(int side)
{
   if (CanCreate(side)) {
      if (side == 0) {
	 created.zero = 1;
	 return(plug0);
      } else {
	 created.one = 1;
	 return(plug1);
      }
   } else
      return(0);
}

inline int HL7Module::CanCreate(int side) const
{
   if (side == 0)
      return(created.zero == 0);
   else
      return(created.one == 0);
}

inline int HL7Module::OwnsPlug(StrPlug *plug) const
{
   if (plug == plug0 && created.zero)
      return(1);
   else if (plug == plug1 && created.one)
      return(1);
   else
      return(0);
}

inline HL7Plug *HL7Module::MakePlug(int side)
{
   return((HL7Plug *)(CreatePlug(side)));
}

inline int HL7Module::AreYouA(int mtype) const
{
   return((mtype == HL7ModuleNum) || StreamModule::AreYouA(mtype));
}

inline HL7Module::HL7Module()
  : message(0), processing(0), messagedone(0), bytesprocessed(0), nexttest(0)
{
   SetSeperator("\nEOM|\n");
   created.zero = created.one = 0;
   plug0 = new HL7Plug(this, 0);
   plug1 = new HL7Plug(this, 1);
}

//=========HL7Plug inlines========

inline HL7Plug::HL7Plug(HL7Module *parnt, int s) : parent(parnt), side(s)
{
   assert(parent != 0 && ((side == 0) || (side == 1)));

   ioflags.rdngfrm = ioflags.wrtngto = 0;
}

inline HL7Plug::~HL7Plug()
{
   if (PluggedInto())
      UnPlug();
}

inline int HL7Plug::AreYouA(int ptype) const
{
   return((ptype == HL7PlugNum) || StrPlug::AreYouA(ptype));
}

inline HL7Module *HL7Plug::ModuleFrom() const
{
   return((HL7Module *)(ParentModule()));
}

#endif
