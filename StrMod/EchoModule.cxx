/* $Header$ */

#ifdef __GNUG__
#pragma implementation "EchoModule.h"
#endif

// For log information, see ChangeLog

// $Revision$


#include "StrMod/EchoModule.h"

const STR_ClassIdent EchoModule::identifier(3UL);
const STR_ClassIdent EchoModule::EPlug::identifier(4UL);

EchoModule::EchoModule() : plugcreated_(false), eplug_(*this)
{
}

EchoModule::~EchoModule()
{
}

bool EchoModule::deletePlug(Plug *plug)
{
   if (i_OwnsPlug(plug))
   {
      plugcreated_ = false;
      plug->unPlug();
      return(true);
   }
   else
   {
      return(false);
   }
}

void EchoModule::plugDisconnected(Plug *plug)
{
   assert(plug == &eplug_);
   assert(plugcreated_);

   setReadableFlagFor(&eplug_, false);
   setWriteableFlagFor(&eplug_, false);
   StreamModule::plugDisconnected(plug);
}

void EchoModule::EPlug::otherIsReadable()
{
   Plug *other = pluggedInto();

   setReadable((other == NULL) ? false : getFlagsFrom(*other).canread_);
}

void EchoModule::EPlug::otherIsWriteable()
{
   Plug *other = pluggedInto();

   setWriteable((other == NULL) ? false : getFlagsFrom(*other).canwrite_);
}

const StrChunkPtr EchoModule::EPlug::i_Read()
{
   StrChunkPtr tmp = readOther();
   Plug *other = pluggedInto();

   if (other != NULL)
   {
      setReadable(getFlagsFrom(*other).canread_);
   }
   else
   {
      setReadable(false);
      setWriteable(false);
   }
   return(tmp);
}

void EchoModule::EPlug::i_Write(const StrChunkPtr &ptr)
{
   writeOther(ptr);

   Plug *other = pluggedInto();

   if (other != NULL)
   {
      setWriteable(getFlagsFrom(*other).canwrite_);
   }
   else
   {
      setReadable(false);
      setWriteable(false);
   }
}
