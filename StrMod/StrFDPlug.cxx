/* $Header$ */

 // $Log$
 // Revision 1.1  1995/07/22 04:46:47  hopper
 // Initial revision
 //
 // -> Revision 0.16  1995/04/14  17:10:13  hopper
 // -> Combined versions 0.15 and 0.15.0.4
 // ->
 // -> Revision 0.15.0.4  1995/04/14  17:09:11  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.15.0.3  1994/06/12  04:40:00  hopper
 // -> 	Made a few changes so that DoWriteableNotify, and DoReadableNotify
 // -> were called at times conformant with the stuff in the Principles document.
 // ->
 // -> Revision 0.15.0.2  1994/05/26  11:28:30  hopper
 // -> Added some debugging statements that are commented out.
 // -> Made StrFDPlug's a little more robust. They handle the 'invalid' state
 // -> better, and don't cause core dumps.
 // ->
 // -> Revision 0.15.0.1  1994/05/16  06:00:56  hopper
 // -> No changes, but I needed a head for the inevitable WinterFire-OS/2
 // -> branch.
 // ->
 // -> Revision 0.15  1994/05/07  03:24:53  hopper
 // -> Changed header files stuff around to be aprroximately right with
 // -> new libraries, and new names & stuff.
 // ->
 // -> Revision 0.14  1992/05/10  23:20:46  hopper
 // -> Fixed it so that the StrFDPlug is unlink from the Dispatcher
 // -> when it's deleted. This could've cause big problems.....
 // ->
 // -> Revision 0.13  1992/04/30  21:26:37  hopper
 // -> Decided to unlink myself from the Dispatcher instead of returning
 // -> -1 because the Dispatcher interprets an unlink as a fatal error,
 // -> and shuts me off from all events. I added
 // -> Dispatch::unlink(fd, DispatcherMask) so I could disconnect myself
 // -> from specific events intead of the whole file descriptor.
 // ->
 // -> Revision 0.12  1992/04/26  01:27:43  hopper
 // -> Added support for blocks of a maximum size. Currently both
 // -> reads and writes use the same maximum, this may change though.
 // ->
 // -> Revision 0.11  1992/04/24  00:23:58  hopper
 // -> Changed several things, including:
 // -> Added support for EOF detection. Special error code (-1) signifies
 // -> EOF.
 // -> Added support for the checkread, and checkwrite flags added to
 // -> StreamFDModule's. See StreamFDModule.h v0.12 for more details.
 // ->
 // -> Revision 0.10  1992/04/21  19:07:29  hopper
 // -> Genesis!
 // ->

#ifndef NO_RcsID
static char _StrFDPlug_CC_rcsID[] = "$Id$";
#endif

#include "StrMod/StreamFDModule.h"
#include <Dispatch/dispatcher.h>
#include "StrMod/StrChunk.h"
#include <unistd.h>
#include <iostream.h>
#include <errno.h>

StrChunk *StrFDPlug::InternalRead()
{
   StreamFDModule *parent = ModuleFrom();
   StrChunk *temp;

   if (parent->fd < 0)
      return(0);
   if (!CanRead()) {
      Dispatcher tempd;

      rdngfrm = 1;
      tempd.link(parent->fd, Dispatcher::ReadMask, this);
      while (parent->fd >= 0 && parent->last_error == 0 && !CanRead())
	 tempd.dispatch();
      if (!CanRead()) {
	 cerr << "Returning 0 from InternalRead() after a blocking read\n";
	 rdngfrm = 0;
	 return(0);
      }
      rdngfrm = 0;
   }
   temp = parent->buffed_read;
   parent->buffed_read = 0;
   if (parent->flags.checkread)
      Dispatcher::instance().link(parent->fd, Dispatcher::ReadMask, this);

/*
   if (temp) {
      cerr << "Returning \"";
      temp->PutIntoFd(2);
      cerr << "\" from InternalRead()\n";
   } else {
      cerr << "Returning 0 from InternalRead()\n";
   }
*/
   return(temp);
}

void StrFDPlug::ReadableNotify()
{
   if (PluggedInto() && CanWrite())
      Write(PluggedInto()->Read());
}

void StrFDPlug::WriteableNotify()
{
   if (PluggedInto() && CanRead())
      PluggedInto()->Write(Read());
}

int StrFDPlug::inputReady(int fd)
{
   StreamFDModule *parent = ModuleFrom();
   int size;

//** If it isn't for me, I'm already buffering, or I have an uncleared
//** error, disconnect me.
   if (fd != parent->fd || parent->buffed_read != 0 ||
       parent->last_error != 0) {
      Dispatcher::instance().unlink(fd, Dispatcher::ReadMask);
      return(0);
   }

   parent->buffed_read = new DataBlockStrChunk();
   size = parent->buffed_read->FillFromFd(parent->fd, 0,
					  parent->max_block_size);
/*
   cerr << "Reading: \"";
   parent->buffed_read->PutIntoFd(2);
   cerr << "\"\n";
   cerr << "parent->buffed_read->GetLastErrno() == "
        << parent->buffed_read->GetLastErrno() << "\"\n";
*/

   if (size <= 0) {
      if (size == 0 && parent->buffed_read->GetLastErrno() == EWOULDBLOCK) {
	 delete parent->buffed_read;
	 parent->buffed_read = 0;
	 return(0);
      } else {
	 if (parent->buffed_read->GetLastErrno() != 0)
	    parent->last_error = parent->buffed_read->GetLastErrno();
	 else if (size == 0)
	    parent->last_error = -1;   // EOF encountered in read.
	 else {
	    assert(parent->buffed_read->GetLastErrno() != 0 ||
		   size == 0);
	 }
	 delete parent->buffed_read;
	 parent->buffed_read = 0;
	 Dispatcher::instance().unlink(fd, Dispatcher::ReadMask);
	 return(0);
      }
   }
   if (!rdngfrm)
      DoReadableNotify();
   return(0);
}

int StrFDPlug::outputReady(int fd)
{
   StreamFDModule *parent = ModuleFrom();

//** If it isn't for me, I've been sucked dry, or I have an uncleared error,
//** then disconnect me.
   if (fd != parent->fd || parent->cur_write == 0 || parent->last_error != 0) {
      Dispatcher::instance().unlink(fd, Dispatcher::WriteMask);
      return(0);
   }

   if (parent->write_pos < parent->cur_write->Length()) {
      int written, length;

      length = parent->cur_write->Length();
      written = parent->cur_write->PutIntoFd(parent->fd, parent->write_pos,
					     parent->max_block_size);
/*
      cerr << "Wrote: \"";
      parent->cur_write->PutIntoFd(2, parent->write_pos, written);
      cerr << "\"\n";
*/
      if (written < 0) {
	 parent->last_error = parent->cur_write->GetLastErrno();
	 Dispatcher::instance().unlink(fd, Dispatcher::WriteMask);
	 return(0);
      } else {
	 parent->write_pos += written;
	 if (parent->write_pos >= length) {
	    delete parent->cur_write;
	    parent->cur_write = 0;
	    parent->write_pos = 0;
	    if (!wrtngto)
	       DoWriteableNotify();
	 }
	 return(0);
      }
   } else {
      delete parent->cur_write;
      parent->cur_write = 0;
      parent->write_pos = 0;
      if (!wrtngto)
	 DoWriteableNotify();
      return(0);
   }
}

int StrFDPlug::exceptionRaised(int fd)
{
   cerr << "StrFDPlugs can't handle exceptions, but I got one, Aigh!\n";
   Dispatcher::instance().unlink(fd, Dispatcher::ExceptMask);
   return(0);
}

void StrFDPlug::timerExpired(long sec, long usec)
{
   cerr << "I got a timer at " << sec << " seconds " << usec <<
           " microseconds\n";
   cerr << "I'm only a StrFDPlug, and I'm not supposed to get timerExpired "
        << "messages, Aigh!\n";
}

bool StrFDPlug::Write(StrChunk *outc)
{
   StreamFDModule *parent = ModuleFrom();

   if (parent->fd < 0)
      return(false);
   if (!CanWrite()) {
      Dispatcher tempd;

      wrtngto = 1;
      tempd.link(parent->fd, Dispatcher::WriteMask, this);
      while (parent->fd >= 0 && parent->last_error == 0 && !CanWrite())
	 tempd.dispatch();
      if (!CanWrite()) {
	 cerr << "Aigh! I'm a StrFDPlug, and I had to throw away something "
	      << "that was supposed to\nbe written out here.\n";
	 wrtngto = 0;
	 return(false);
      }
      wrtngto = 0;
   }
   parent->cur_write = outc;
   parent->write_pos = 0;
/*
   cerr << "Set parent->cur_write to \"";
   parent->cur_write->PutIntoFd(2);
   cerr << "\"\n";
*/
   if (parent->flags.checkwrite)
      Dispatcher::instance().link(parent->fd, Dispatcher::WriteMask, this);
   return(true);
}

StrFDPlug::StrFDPlug(StreamFDModule *parent)
  : smod(parent), rdngfrm(0), wrtngto(0)
{
   if (parent->fd >= 0) {
      if (parent->flags.checkwrite)
	 Dispatcher::instance().link(parent->fd, Dispatcher::WriteMask, this);
      if (parent->flags.checkread)
	 Dispatcher::instance().link(parent->fd, Dispatcher::ReadMask, this);
   }
}

StrFDPlug::~StrFDPlug()
{
   StreamFDModule *parent = ModuleFrom();

   if (parent->cur_write)
      delete parent->cur_write;
   if (parent->buffed_read)
      delete parent->buffed_read;
   if (parent->fd >= 0)
      Dispatcher::instance().unlink(parent->fd);
}
