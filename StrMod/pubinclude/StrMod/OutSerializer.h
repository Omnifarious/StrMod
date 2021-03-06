#ifndef _STR_OutSerializer_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog
//
// Revision 1.2  1998/06/02 00:30:53  hopper
// Fixed a small bug that caused adding a 2 bytes value to not work
// correctly.
//
// Revision 1.1  1997/04/11 17:44:15  hopper
// Added OutSerializer class for simple serialization of basic data types.
//

#include <StrMod/BufferChunk.h>
#include <LCore/GenTypes.h>
#include <cstring>
#include <string>

#define _STR_OutSerializer_H_

namespace strmod {
namespace strmod {

/** \class OutSerializer OutSerializer.h StrMod/OutSerializer.h
 * A class that marshals data into a canonical binary format.
 *
 * This class serializes integers of various sizes and strings into a canonical
 * binary format that can be given to an InSerializer on a completely different
 * platform to read the data back in.
 */
class OutSerializer
{
 public:
   struct State;

   OutSerializer(size_t suggested_size);
   /** \brief Use this as the external state to save into on destruction, only
    * use this if you've read the directions.  :-)
    *
    * This class can be much more efficient if it's always instantiated locally.
    * Sometimes though, the state will need to be saved across function calls.
    * That's where this constructor comes in.
    *
    * If the takeChunk function is called, a flag is set in the external state
    * (that is also set when the external state is initially constructed) that
    * tells this OutSerializer constructor that it needs to reset the state.
    */
   OutSerializer(State &savedstate);
   OutSerializer();
   virtual ~OutSerializer();

   inline void addS1Byte(lcore::S1Byte num);
   inline void addU1Byte(lcore::U1Byte num);

   inline void addS2Byte(lcore::S2Byte num);
   inline void addU2Byte(lcore::U2Byte num);

   inline void addS4Byte(lcore::S4Byte num);
   inline void addU4Byte(lcore::U4Byte num);

   void addBool(bool val)                    { addU1Byte(lcore::U1Byte(val)); }

   void addString(const ::std::string &str);
   void addString(const char *str);  // C style string.

   inline void addRaw(const void *data, size_t len);

   inline void setMinSuggestedSize(size_t size);

   //: It is an error to call this function more than once.  The chunk you get
   //: has exactly the number of bytes you put in.
   BufferChunk *takeChunk();

   //: Holds the state of the OutSerializer so it can be saved and restored.
   // <p>All inline functions here are defined in the .cxx fileI have heard th
   class State {
    public:
      friend class OutSerializer;
      //: Assumes ownership of fact.
      inline State(BufferChunk::Factory *fact);

    private:
      BufferChunk::Factory * const fact_;
      BufferChunk *cur_chunk_;
      lcore::U1Byte *buf_;
      size_t chnklen_;
      size_t cur_pos_;

      inline State(const State &b);
      inline const State &operator =(const State &b) ;
   };

 private:
   struct State state_;
   struct State * const external_state_;

   void resizeChunk(size_t newsize);
};

//-----------------------------inline functions--------------------------------

inline void OutSerializer::setMinSuggestedSize(size_t size)
{
   if (size > state_.chnklen_) {
      resizeChunk(size);
   }
}

inline void OutSerializer::addS1Byte(lcore::S1Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 1);

   lcore::hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_++;
}

inline void OutSerializer::addU1Byte(lcore::U1Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 1);

   lcore::hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_++;
}

inline void OutSerializer::addS2Byte(lcore::S2Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 2);

   lcore::hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 2;
}

inline void OutSerializer::addU2Byte(lcore::U2Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 2);

   lcore::hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 2;
}

inline void OutSerializer::addS4Byte(lcore::S4Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 4);

   lcore::hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 4;
}

inline void OutSerializer::addU4Byte(lcore::U4Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 4);

   lcore::hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 4;
}

inline void OutSerializer::addRaw(const void *data, size_t len)
{
   setMinSuggestedSize(state_.cur_pos_ + len);

   memcpy(state_.buf_ + state_.cur_pos_, data, len);
   state_.cur_pos_ += len;
}

//---

inline OutSerializer &operator <<(OutSerializer &os, lcore::S1Byte num)
{
   os.addS1Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, lcore::U1Byte num)
{
   os.addU1Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, lcore::S2Byte num)
{
   os.addS2Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, lcore::U2Byte num)
{
   os.addU2Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, lcore::S4Byte num)
{
   os.addS4Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, lcore::U4Byte num)
{
   os.addU4Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, const ::std::string &str)
{
   os.addString(str);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, const char *str)
{
   os.addString(str);
   return(os);
}

}  // namespace strmod
}  // namespace strmod

#endif
