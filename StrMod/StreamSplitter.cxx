/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "StreamSplitter.h"
#endif

#include "StrMod/StreamSplitter.h"
#include <assert.h>
#include <iostream.h>

static StreamSplitterModule test;
const STR_ClassIdent StreamSplitterModule::identifier(10UL);
const STR_ClassIdent StreamSplitterModule::SPPlug::identifier(11UL);

StreamSplitterModule::StreamSplitterModule()
     : inplug_(*this, SideIn),
       outplug_(*this, SideOut),
       biplug_(*this, SideBiDir)
{
   flags_.inmade = flags_.outmade = flags_.bimade = false;
}

StreamSplitterModule::~StreamSplitterModule()
{
}

bool_val StreamSplitterModule::deletePlug(Plug *plug)
{
   // If I don't own the plug, I can't delete it.
   if (!ownsPlug(plug))
   {
      return(false);
   }

   if (&inplug_ == plug)
   {
      flags_.inmade = 0;
   }
   else if (&outplug_ == plug)
   {
      flags_.outmade = 0;
   }
   else if (&biplug_ == plug)
   {
      flags_.bimade = 0;
   }

   if (plug->pluggedInto() != NULL)
   {
      plug->unPlug();
   }

   return(true);
}

void StreamSplitterModule::plugDisconnected(Plug *plug)
{
   switch (plug->side())
   {
    case SideIn:
      biplug_.setReadable(false);
      break;
    case SideOut:
      biplug_.setWriteable(false);
      break;
    case SideBiDir:
      inplug_.setWriteable(false);
      outplug_.setReadable(false);
      break;
   }
   StreamModule::plugDisconnected(plug);
}

StreamModule::Plug *StreamSplitterModule::i_MakePlug(int side)
{
   switch (side) {
    case SideIn:
      if (! flags_.inmade)
      {
	 flags_.inmade = true;
	 return(&inplug_);
      }
      break;
    case SideOut:
      if (! flags_.outmade)
      {
	 flags_.outmade = true;
	 return(&outplug_);
      }
      break;
    case SideBiDir:
      if (!flags_.bimade)
      {
	 flags_.bimade = true;
	 return(&biplug_);
      }
      // default: case handle by switch statement drop through.
   }
   return(NULL);
}

const StrChunkPtr StreamSplitterModule::SPPlug::i_Read()
{
   SPPlug *partner = getReadPartner();
   assert(partner != NULL);
   assert(partner->pluggedInto() != NULL);

   Plug *other = partner->pluggedInto();
   setIsReading(*other, true);
   const StrChunkPtr chunk = partner->readOther();
   setIsReading(*other, false);
   other = partner->pluggedInto();

   setReadable((other == NULL) ? false : getFlagsFrom(*other).canread_);

   return(chunk);
}

void StreamSplitterModule::SPPlug::i_Write(const StrChunkPtr &ptr)
{
   SPPlug *partner = getWritePartner();
   assert(partner != NULL);
   assert(partner->pluggedInto() != NULL);

   Plug *other = partner->pluggedInto();
   setIsWriting(*other, true);
   partner->writeOther(ptr);
   setIsWriting(*other, false);
   other = partner->pluggedInto();

   setWriteable((other == NULL) ? false : getFlagsFrom(*other).canwrite_);
}

void StreamSplitterModule::SPPlug::otherIsReadable()
{
//     cerr << "plug side: " << side_ << " otherIsReadable\n";
   SPPlug *partner = getWritePartner();
//     cerr << "partner == " << partner;
//     if (partner != NULL)
//     {
//        cerr << " && partner->side() == " << partner->side();
//     }
//     cerr << "\n";

   if (partner != NULL)
   {
      Plug *other = pluggedInto();

      partner->setReadable((other == NULL) ?
			   false :
			   getFlagsFrom(*other).canread_);
   }
}

void StreamSplitterModule::SPPlug::otherIsWriteable()
{
//     cerr << "plug side: " << side_ << " otherIsWriteable\n";
   SPPlug *partner = getReadPartner();
//     cerr << "partner == " << partner;
//     if (partner != NULL)
//     {
//        cerr << " && partner->side() == " << partner->side();
//     }
//     cerr << "\n";

   if (partner != NULL)
   {
      Plug *other = pluggedInto();

      partner->setWriteable((other == NULL) ?
			    false :
			    getFlagsFrom(*other).canwrite_);
   }
}
