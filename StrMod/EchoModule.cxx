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
      plug->unPlug();
      plugcreated_ = false;
      return(true);
   }
   else
   {
      return(false);
   }
}

void EchoModule::EPlug::otherIsReadable()
{
   Plug *other = pluggedInto();

   if (other != NULL && getFlagsFrom(*other).canread_)
   {
      setReadable(true);
   }
}

void EchoModule::EPlug::otherIsWriteable()
{
   Plug *other = pluggedInto();

   if (other != NULL && getFlagsFrom(*other).canwrite_)
   {
      setWriteable(true);
   }
}

inline const StrChunkPtr EchoModule::EPlug::i_Read()
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

inline void EchoModule::EPlug::i_Write(const StrChunkPtr &ptr)
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
