#ifndef _STR_OutSerializer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

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
#include <string.h>
#include <string>

#define _STR_OutSerializer_H_

class OutSerializer {
 public:
   struct State;

   OutSerializer(size_t suggested_size);
   //: Use this as the external state to save into on destruction.  Only use
   //: this if you've read the directions.  :-)
   // <p>This class can be much more efficient if it's always instantiated
   // locally.  Sometimes though, the state will need to be saved across
   // function calls.  That's where this constructor comes in.</p>
   // <p>If the <code>takeChunk</code> function is called, a flag is set in
   // the external state (that is also set when the external state is
   // initially constructed) that tells this OutSerializer constructor that it
   // needs to reset the state.</p>
   OutSerializer(State &savedstate);
   OutSerializer();
   virtual ~OutSerializer();

   inline void addS1Byte(S1Byte num);
   inline void addU1Byte(U1Byte num);

   inline void addS2Byte(S2Byte num);
   inline void addU2Byte(U2Byte num);

   inline void addS4Byte(S4Byte num);
   inline void addU4Byte(U4Byte num);

   void addBool(bool val)                            { addU1Byte(U1Byte(val)); }

   void addString(const string &str);
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
      U1Byte *buf_;
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

inline void OutSerializer::addS1Byte(S1Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 1);

   hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_++;
}

inline void OutSerializer::addU1Byte(U1Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 1);

   hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_++;
}

inline void OutSerializer::addS2Byte(S2Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 2);

   hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 2;
}

inline void OutSerializer::addU2Byte(U2Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 2);

   hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 2;
}

inline void OutSerializer::addS4Byte(S4Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 4);

   hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 4;
}

inline void OutSerializer::addU4Byte(U4Byte num)
{
   setMinSuggestedSize(state_.cur_pos_ + 4);

   hton(num, state_.buf_ + state_.cur_pos_);
   state_.cur_pos_ += 4;
}

inline void OutSerializer::addRaw(const void *data, size_t len)
{
   setMinSuggestedSize(state_.cur_pos_ + len);

   memcpy(state_.buf_ + state_.cur_pos_, data, len);
   state_.cur_pos_ += len;
}

//---

inline OutSerializer &operator <<(OutSerializer &os, S1Byte num)
{
   os.addS1Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, U1Byte num)
{
   os.addU1Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, S2Byte num)
{
   os.addS2Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, U2Byte num)
{
   os.addU2Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, S4Byte num)
{
   os.addS4Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, U4Byte num)
{
   os.addU4Byte(num);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, const string &str)
{
   os.addString(str);
   return(os);
}

inline OutSerializer &operator <<(OutSerializer &os, const char *str)
{
   os.addString(str);
   return(os);
}

#endif
