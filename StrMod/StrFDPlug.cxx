/* $Header$ */

 // $Log$
 // Revision 1.9  1998/06/02 00:33:12  hopper
 // Removed GNU specific return value optimization stuff.  A good compiler
 // should do it automagically.
 //
 // Revision 1.8  1996/10/23 10:12:53  hopper
 // Fixed an = vs. == bug that caused StreamFDModule to stop being able to
 // write.
 //
 // Revision 1.7  1996/09/21 20:01:37  hopper
 // Moved the guts of the file I/O (the stuff that actually reads on writes to
 // the file descriptor) out of StrFDPlug, and put it into
 // StreamFDModule::DoWriteFD, and StreamFDModule::DoReadFD.  This makes
 // StreamFDModule a little easier to inherit from.
 //
 // Revision 1.6  1996/09/02 23:09:52  hopper
 // Added a crude attempt at fixing deficiencies in AIX 3.2.5's include
 // files with respect to socket calls.
 //
 // Revision 1.5  1996/07/07 20:56:48  hopper
 // Added, then commented out some debugging.  I left it there because it
 // might be useful someday.
 //
 // Revision 1.4  1996/07/05 18:47:13  hopper
 // Changed to use new StrChunkPtr stuff.
 //
 // Revision 1.3  1996/02/19 03:54:15  hopper
 // Minor cleanup changes made while debugging.
 //
 // Revision 1.2  1995/07/23 04:00:33  hopper
 // Added #include <string.h> in various places to avoid triggering a bug
 // in libg++ 2.7.0
 //
 // Revision 1.1.1.1  1995/07/22 04:46:47  hopper
 // Imported sources
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
#include "StrMod/DBStrChunk.h"
#include "StrMod/GroupVector.h"
#include <string.h>
#include <unistd.h>
#include <iostream.h>
#include <errno.h>
#include "config.h"

#ifdef NEED_WRITEV_DECL
extern "C" int writev(int fd, const struct iovec *iov, int iovCount);
#endif
#ifndef MAXIOVCNT
#  define MAXIOVCNT (16U)
#endif

const StrChunkPtr StrFDPlug::InternalRead()
{
//   cerr << "In StrFDPlug::InternalRead()\n";

   StrChunkPtr temp;
   StreamFDModule *parent = ModuleFrom();

   if (parent->fd < 0)
      return(0);
   if (!CanRead()) {
      Dispatcher tempd;

//      cerr << "Here 1\n";
      rdngfrm = 1;
      tempd.link(parent->fd, Dispatcher::ReadMask, this);
      while (parent->fd >= 0 && parent->last_error == 0 && !CanRead())
	 tempd.dispatch();
      if (!CanRead()) {
	 cerr << "Returning 0 from InternalRead() after a blocking read\n";
	 rdngfrm = 0;
	 // temp, having not been set, should be NULL here.
	 return(temp);
      }
      rdngfrm = 0;
   }
//   cerr << "Here 2\n";
   temp = parent->buffed_read;
   parent->buffed_read.ReleasePtr();
   if (parent->flags.checkread) {
      Dispatcher::instance().link(parent->fd, Dispatcher::ReadMask, this);
   }

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
   if (PluggedInto() && PluggedInto()->CanRead() && CanWrite())
      Write(PluggedInto()->Read());
}

void StrFDPlug::WriteableNotify()
{
   if (PluggedInto() && PluggedInto()->CanWrite() && CanRead())
      PluggedInto()->Write(Read());
}

int StrFDPlug::inputReady(int fd)
{
   StreamFDModule *parent = ModuleFrom();

//** If it isn't for me, I'm already buffering, or I have an uncleared
//** error, disconnect me.
   if (fd != parent->fd || parent->buffed_read ||
       parent->last_error != 0) {
      Dispatcher::instance().unlink(fd, Dispatcher::ReadMask);
      return(0);
   }

   parent->DoReadFD();

   if (!rdngfrm && parent->buffed_read) {
      DoReadableNotify();
   }
   return(0);
}

// This should only really be called from StrFDPlug::inputReady since
// that's the function that makes sure all the preconditions are true
// first.
void StreamFDModule::DoReadFD()
{
   assert(!buffed_read);
   assert(last_error == 0);
   assert(fd >= 0);

   int size;

   {
      // A normal pointer offers a speed advantage, and we don't know whether
      // we want to set buffed_read until the read succeeds.
      unsigned int maxsize = GetMaxBlockSize();
      DataBlockStrChunk *dbchunk = new DataBlockStrChunk(maxsize);

      errno = 0;
      size = read(fd, dbchunk->GetVoidP(), maxsize);
      if (size > 0) {
	 dbchunk->Resize(size);
	 buffed_read = dbchunk;
//	 cerr << "Reading: \"";
//	 cerr.write(dbchunk->GetVoidP(), dbchunk->Length());
//	 cerr << "\"\n";
	 return;
      } else {
	 delete dbchunk;
      }
   }

   // The following code assumes size <= 0 (i.e. there's been an error).
   assert(size <= 0);

   // EWOULDBLOCK shouldn't really happen, but is an OK error, so ignore it.
   if (!((size < 0) && (errno == EWOULDBLOCK))) {
      if (errno != 0) {
	 last_error = errno;
      } else if (size == 0) {
	 last_error = -1;   // EOF encountered in read.
      } else {
	 assert(errno != 0 || size == 0);
      }
      Dispatcher::instance().unlink(fd, Dispatcher::ReadMask);
   }
}

int StrFDPlug::outputReady(int fd)
{
   StreamFDModule *parent = ModuleFrom();

//** If it isn't for me, I've been sucked dry, or I have an uncleared error,
//** then disconnect me.
   if (fd != parent->fd || !parent->cur_write || parent->last_error != 0) {
      Dispatcher::instance().unlink(fd, Dispatcher::WriteMask);
      return(0);
   }

   parent->DoWriteFD();

   if (!wrtngto && (parent->fd >= 0)
       && (parent->last_error == 0) && !(parent->cur_write)) {
      DoWriteableNotify();
   }
   return(0);
}

// This should only really be called from StrFDPlug::outputReady since
// that's the function that makes sure all the preconditions are true
// first.
void StreamFDModule::DoWriteFD()
{
   assert(cur_write);
   assert(last_error == 0);
   assert(fd >= 0);

   if (write_vec == 0) {
      write_vec = new GroupVector(cur_write->NumSubGroups());
      cur_write->SimpleFillGroupVec(*write_vec);
   }

   assert(write_vec != 0);

   size_t length = write_vec->TotalLength();

   if (write_pos < length) {
      int written;
      bool result;
      GroupVector::IOVecDesc ioveclst;

      result = write_vec->FillIOVecDesc(write_pos, ioveclst);
      assert(result);
      assert(ioveclst.iovcnt > 0);
      if (ioveclst.iovcnt > MAXIOVCNT) {
	 ioveclst.iovcnt = MAXIOVCNT;
      }
      errno = 0;
      written = writev(fd, ioveclst.iov, ioveclst.iovcnt);
/*
      cerr << "Wrote: \"";
      parent->cur_write->PutIntoFd(2, parent->write_pos, written);
      cerr << "\"\n";
*/
      if (written < 0) {
	 last_error = errno;
	 Dispatcher::instance().unlink(fd, Dispatcher::WriteMask);
	 return;
      } else {
	 write_pos += written;
      }
   }
   if (write_pos >= length) {
      if (write_vec) {
	 delete write_vec;
	 write_vec = 0;
      }
      cur_write.ReleasePtr();
      write_pos = 0;
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

bool StrFDPlug::Write(const StrChunkPtr &outc)
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
   parent->write_vec = new GroupVector(outc->NumSubGroups());
   outc->SimpleFillGroupVec(*(parent->write_vec));
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
     : StrPlug(parent), rdngfrm(0), wrtngto(0)
{
   if (parent->fd >= 0) {
      if (parent->flags.checkwrite) {
	 Dispatcher::instance().link(parent->fd, Dispatcher::WriteMask, this);
      }
      if (parent->flags.checkread) {
	 Dispatcher::instance().link(parent->fd, Dispatcher::ReadMask, this);
      }
   }
}

StrFDPlug::~StrFDPlug()
{
   StreamFDModule *parent = ModuleFrom();

   UnPlug();
   parent->cur_write.ReleasePtr();
   if (parent->write_vec) {
      delete parent->write_vec;
      parent->write_vec = 0;
   }
   parent->buffed_read.ReleasePtr();
   if (parent->fd >= 0)
      Dispatcher::instance().unlink(parent->fd);
}
