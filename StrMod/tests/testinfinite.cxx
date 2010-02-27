/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Header$ */

// For a list of changes, see ../ChangeLog

#include <StrMod/StreamSplitter.h>
#include <StrMod/StreamFDModule.h>
#include <StrMod/FDUtil.h>
#include <StrMod/InfiniteModule.h>
#include <StrMod/PreAllocBuffer.h>
#include <UniEvent/UNIXpollManagerImp.h>
#include <UniEvent/SimpleDispatcher.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

static void handle_intr(int);

int main(int argc, char *argv[])
{
   signal(SIGINT, handle_intr);

   int error = 0;

   if (FDUtil::setNonBlock(0, error))
   {
      if (!FDUtil::setNonBlock(1, error))
      {
	 cerr << "Couldn't set stdout to non-blocking.\n";
	 return(1);
      }
   }
   else
   {
      cerr << "Couldn't set stdin to non-blocking.\n";
      return(1);
   }

   UNISimpleDispatcher disp;
   UNIXpollManagerImp pm(&disp);
   StreamFDModule sin(0, pm, StreamFDModule::CheckRead, false);
   StreamFDModule sout(1, pm, StreamFDModule::CheckWrite, false);
   StreamSplitterModule splitter;
   PreAllocBuffer<15> *chnk = new PreAllocBuffer<15>;
   chnk->resize(14);
   memcpy(chnk->getVoidP(), "George Orwell\n", 14);
   InfiniteModule inf(chnk);

   sin.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideIn))
      );
   sout.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideOut))
      );
   inf.makePlug(0)->plugInto(
      *(splitter.makePlug(StreamSplitterModule::SideBiDir))
      );

   while (!sin.hasError())
   {
      disp.DispatchEvents(1);
   }
   return(0);
}

static void handle_intr(int sig)
{
   int error = 0;

   FDUtil::setBlock(0, error);
   FDUtil::setBlock(1, error);
   signal(sig, SIG_DFL);
   kill(getpid(), sig);
}
