#ifndef _STR_StreamProcessor_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log, see ../ChangeLog
//
// Revision 1.2  1996/09/11 23:18:08  hopper
// Changed StreamProcessor::CanAddIncoming to use StrChunkPtr operator !
// instead of comparison to 0.
//
// Revision 1.1  1996/09/02 23:33:54  hopper
// Created abstract class (class StreamProcessor) for handling simple
// dataflows where one grabbed incoming data, did stuff to it, and sent
// the result along.
//

#include <cassert>
#include <LCore/GenTypes.h>
#include <StrMod/StrChunkPtr.h>

#define _STR_StreamProcessor_H_

namespace strmod {
namespace strmod {

/** \class StreamProcessor StreamProcessor.h StrMod/StreamProcessor.h
 * Describes a simple non-active processor of a unidirectional data stream
 * that has one input and one output.
 *
 * Non-active means that this things readable or writeable status can't change
 * unless it's a result of being read or written to.
 *
 * This class is intended to be used with a ProcessorModule to create modules
 * that do some sort of processing on every chunk that passes through.
 *
 * A prime example of this sort of thing are streams that re-chunk the data
 * passing through them according to some criteria.  Another example is a
 * stream which simply prepends a header of some sort to every chunk coming
 * through.
 *
 * Another prime example is a process in a Unix pipeline.
 */
class StreamProcessor
{
 public:
   //! Abstract base classes don't have substansive constructors.
   inline StreamProcessor();
   //! Abstract base classes don't have substansive destructors.
   virtual ~StreamProcessor();

   /** Can you put data into this thing?
    * Note that this is \c !incoming_.  This means that if you need
    * more data to complete your processing, your must clear incoming_ and
    * store the partially processed data someplace else, like
    * \c outgoing_.
    */
   inline bool canWriteTo() const;
   //! Shove in some data.  Must not be called when !canWriteTo().
   inline void writeTo(const StrChunkPtr &chnk);
   /** Can you get any data from this thing?
    * Note that this is \c outgoing_ready_.  It's a gross error for
    * <code>!outgoing_ && outgoing_ready_</code>.  Set \c outgoing_ready_ when
    * the data in \c outgoing_ is ready to go.
    */
   inline bool canReadFrom() const;
   //! Pull out some data.  Must not be called when !canReadFrom().
   inline const StrChunkPtr readFrom();

 protected:
   StrChunkPtr incoming_;  //!< Where to find the incoming data when processIncoming() is called.
   StrChunkPtr outgoing_;  //!< Where to stick data that's ready to go out.
   bool outgoing_ready_;   //!< Set this when the data in \c outgoing_ is actually ready to go out.

   /** Do something with your incoming_ data.
    * \pre <code>(incoming_ && !outoing_ready_)</code> will
    * <strong>always</strong> be true when entering this function, meaning
    * that incoming_ points at a valid chunk.
    *
    * \post A post condition of this function is <code>(!incoming_ ||
    * (outgoing_ready_ && outgoing_))</code>.
    */
   virtual void processIncoming() = 0;

 private:
   // Inhibit accidental copying.
   StreamProcessor(const StreamProcessor &b);
   void operator =(const StreamProcessor &b);
};

//-----------------------------inline functions--------------------------------

inline StreamProcessor::StreamProcessor()
     : outgoing_ready_(false)
{
}

inline bool StreamProcessor::canWriteTo() const
{
   return(!incoming_);
}

inline void StreamProcessor::writeTo(const StrChunkPtr &chnk)
{
   assert(canWriteTo());
   incoming_ = chnk;
   if (!outgoing_ready_) {
      processIncoming();
      assert(!incoming_ || (outgoing_ready_ && outgoing_));
   }
}

inline bool StreamProcessor::canReadFrom() const
{
   return(outgoing_ready_);
}

inline const StrChunkPtr StreamProcessor::readFrom()
{
   assert(outgoing_ready_);

   StrChunkPtr tmp;

   tmp.swap(outgoing_);
   outgoing_ready_ = false;
   if (incoming_)
   {
      processIncoming();
      assert(!incoming_ || (outgoing_ready_ && outgoing_));
   }
   return(tmp);
}

}  // namespace strmod
}  // namespace strmod

#endif
