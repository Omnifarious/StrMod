/* $Header$ */

// $Log$
// Revision 1.2  1996/09/03 01:58:25  hopper
// Commented out all kinds of debugging stuff.
//
// Revision 1.1  1996/09/02 23:31:12  hopper
// Added class that would turn two StreamProcessors into a StreamModule
// that used one StreamProcessor for the data flowing in each direction.
//

#ifdef __GNUG__
#  pragma implementation "ProcessorModule.h"
#endif

#include <StrMod/ProcessorModule.h>
// #include <iostream.h>

const STR_ClassIdent ProcessorModule::identifier(29UL);
const STR_ClassIdent ProcessorModule::Plug::identifier(30UL);

ProcessorModule::ProcessorModule(StreamProcessor *one, StreamProcessor *other)
     : side_(this, otherside_, OneSide, one, other),
       otherside_(this, side_, OtherSide, other, one)
{
   pulled_.side = false;
   pulled_.otherside = false;
}

ProcessorModule::~ProcessorModule()
{
   if (side_.PluggedInto()) {
      side_.UnPlug();
   }
   if (otherside_.PluggedInto()) {
      otherside_.UnPlug();
   }
}

bool ProcessorModule::DeletePlug(StrPlug *plug)
{
   const StrPlug * const sidep = &side_;
   const StrPlug * const othersidep = &otherside_;

   if (plug == sidep && pulled_.side) {
      side_.UnPlug();
      pulled_.side = false;
      return(true);
   } else if (plug == othersidep && pulled_.otherside) {
      otherside_.UnPlug();
      pulled_.otherside = false;
      return(true);
   }
   return(false);
}

StrPlug *ProcessorModule::CreatePlug(int side)
{
   assert((side == OneSide) || (side == OtherSide));

   PlugSide rside = static_cast<PlugSide>(side);

   switch (rside) {
    case OneSide:
      if (!pulled_.side) {
	 return(&side_);
      }
      break;
    case OtherSide:
      if (!pulled_.otherside) {
	 return(&otherside_);
      }
      break;
   }
   return(0);
}

bool ProcessorModule::CanCreate(int side) const
{
   switch (side) {
    case OneSide:
      return(!pulled_.side);
      break;
    case OtherSide:
      return(!pulled_.otherside);
      break;
   }
   return(false);
}

bool ProcessorModule::Plug::i_Write(const StrChunkPtr &chnk)
{
   assert(chnk);
//   cerr << "ProcessorModule::Plug::i_Write called ("
//	<< (Side() == OneSide ? "OneSide" :
//	    (Side() == OtherSide ? "OtherSide" : "Unknown")) << ")\n";

   bool oldoutgoing = inproc_->CanPullOutgoing();
   inproc_->AddIncoming(chnk);
   if (!oldoutgoing && inproc_->CanPullOutgoing()) {
      otherplug_.CheckNotify();
   }

   return(true);
}

const StrChunkPtr ProcessorModule::Plug::i_InternalRead()
{
//   cerr << "ProcessorModule::Plug::i_InternalRead called!\n";

   bool oldincoming = outproc_->CanAddIncoming();
   StrChunkPtr chnk = outproc_->PullOutgoing();

   if (!oldincoming && outproc_->CanAddIncoming()) {
      otherplug_.CheckNotify();
   }

   return(chnk);
}

void ProcessorModule::Plug::CheckNotify()
{
//   cerr << "ProcessorModule::Plug::CheckNotify called!\n";
   if (CanWrite()) {
//      cerr << "ProcessorModule::Plug::DoWriteableNotify called!\n";
      DoWriteableNotify();
   }
   if (CanRead()) {
//      cerr << "ProcessorModule::Plug::DoReadableNotify called!\n";
      DoReadableNotify();
   }
}

