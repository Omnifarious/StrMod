/* -*-c-file-style: "hopper";-*- */

/*
 * Copyright 2000-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "UseTrackingVisitor.h"
#endif

#include "StrMod/UseTrackingVisitor.h"
#include "StrMod/StackSwapper.h"
#include "StrMod/StrChunk.h"
#include "StrMod/StrChunkPtr.h"
#include <cassert>

namespace strmod {
namespace strmod {

/*!
 * @param ignorezeros Should the use_* Template Methods be called for
 * StrChunks that have no used bytes or data sections that have no
 * used bytes?
 */
UseTrackingVisitor::UseTrackingVisitor(bool ignorezeros)
     : curpos_(0), ignorezeros_(ignorezeros)
{
}

void UseTrackingVisitor::visitStrChunk(const StrChunkPtr &chunk)
{
//   cerr << "visitStrChunk 1: Visiting " << chunk.GetPtr() << " && curpos_ == " << curpos_ << "\n";
   assert(chunk);
   if (ignorezeros_ && (curpos_ >= curext_.endOffset()))
   {
      return;
   }
   LinearExtent::length_t chunklen = chunk->Length();
   LinearExtent used = computeUsed(LinearExtent(0, chunklen));
   if ((used.Length() != 0) || !ignorezeros_)
   {
      use_visitStrChunk(chunk, used);
   }
   do_acceptVisitor(chunk, used);
   curpos_ += chunklen;
//   cerr << "visitStrChunk 1: Leaving " << chunk.GetPtr() << " && curpos_ == " << curpos_ << "\n";
}

void UseTrackingVisitor::visitStrChunk(const StrChunkPtr &chunk,
                                        const LinearExtent &used)
{
//   cerr << "visitStrChunk 2: Visiting " << chunk.GetPtr() << " && curpos_ == " << curpos_ << "\n";
   assert(chunk);
   if (ignorezeros_ && (curpos_ >= curext_.endOffset()))
   {
      return;
   }
   LinearExtent::length_t chunklen = chunk->Length();
   LinearExtent chnkused =
      computeUsed(LinearExtent(0, chunklen).SubExtent(used));
   if ((used.Length() > 0) || !ignorezeros_)
   {
      use_visitStrChunk(chunk, chnkused);
   }
   do_acceptVisitor(chunk, chnkused);
   curpos_ += used.Length();
//   cerr << "visitStrChunk 2: Leaving " << chunk.GetPtr() << " && curpos_ == " << curpos_ << "\n";
}

void UseTrackingVisitor::visitDataBlock(const void *data, size_t len)
{
//   cerr << "visitDataBlock: Visiting " << data << " && curpos_ == " << curpos_ << "\n";
   assert(data != NULL);
   if (ignorezeros_ && (curpos_ >= curext_.endOffset()))
   {
      return;
   }
   LinearExtent used = computeUsed(LinearExtent(0, len));
   if ((used.Length() > 0) || !ignorezeros_)
   {
      if (used.Offset() != 0)
      {
//         cerr << "used.Offset() == " << used.Offset() << " && used.Length() == " << used.Length() << "\n";
         use_visitDataBlock(static_cast<const unsigned char *>(data) +
                            used.Offset(),
                            used.Length(),
                            data, len);
      }
      else
      {
//         cerr << "used.Length() == " << used.Length() << "\n";
         use_visitDataBlock(data, used.Length(), data, len);
      }
   }
   curpos_ += len;
//   cerr << "visitDataBlock: Leaving " << data << " && curpos_ == " << curpos_ << "\n";
}

const LinearExtent UseTrackingVisitor::computeUsed(const LinearExtent &used)
{
   LinearExtent chunkext(curpos_, used.Length());
   LinearExtent overlap = chunkext.intersection(curext_);
   chunkext = overlap;
   assert(chunkext.Offset() >= curpos_);
   chunkext.MoveLeft(curpos_);
   return used.SubExtent(chunkext);
}

void UseTrackingVisitor::do_acceptVisitor(const StrChunkPtr &chunk,
                                          const LinearExtent &chunkext)
{
   // curchnk_ is chunk for duration of function.  Automatically set back to
   // old value when function exits for any reason.
   StackSwapper<StrChunkPtr> s1(curchnk_, chunk);
   StackSwapper<LinearExtent::off_t> s2(curpos_, 0);
   StackSwapper<LinearExtent> s3(curext_, chunkext);
   call_acceptVisitor(chunk);
}

}  // End namespace strmod
}  // End namespace strmod
