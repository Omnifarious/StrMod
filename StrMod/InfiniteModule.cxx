/* $Header$ */

// For a log, see ./ChangeLog

#ifdef __GNUG__
#  pragma implementation "InfiniteModule.h"
#endif

#include "StrMod/InfiniteModule.h"
#include "StrMod/PreAllocBuffer.h"
#include <cstring>  // memset

const STR_ClassIdent InfiniteModule::identifier(41UL);
const STR_ClassIdent InfiniteModule::IPlug::identifier(42UL);

InfiniteModule::InfiniteModule(const StrChunkPtr &chnk)
     : feed_(chnk), plug_created_(false), plug_(*this)
{
}

InfiniteModule::InfiniteModule()
     : plug_created_(false), plug_(*this)
{
   const int bufsize = 8192;
   PreAllocBuffer<bufsize> *buf = new PreAllocBuffer<bufsize>;

   buf->resize(bufsize);
   memset(buf->getVoidP(), '\0', bufsize);
   feed_ = buf;
}

InfiniteModule::~InfiniteModule()
{
}

bool InfiniteModule::deletePlug(Plug *plug)
{
   if ((&plug_ == plug) && plug_created_)
   {
      plug_created_ = false;
      plug_.unPlug();
      setReadableFlagFor(&plug_, false);
      setWriteableFlagFor(&plug_, false);
      return(true);
   }
   else
   {
      return(false);
   }
}

StreamModule::Plug *InfiniteModule::i_MakePlug(int side)
{
   plug_created_ = true;
   setReadableFlagFor(&plug_, true);
   setWriteableFlagFor(&plug_, true);
   return(&plug_);
}
