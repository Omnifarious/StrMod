#ifndef _STR_CharChopper_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/09/02 23:28:27  hopper
// Added CharChopper class so users would have a simple class that would
// break up and recombine streams using whatever character they chose as
// a separator.
//

#include <StrMod/StreamProcessor.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/DBStrChunk.h>

#define _STR_CharChopper_H_

class GroupVector;

class CharChopper : public StreamProcessor {
 public:
   typedef StreamProcessor parentclass;
   static const STR_ClassIdent identifier;

   CharChopper(char chopchar) : chopchar_(chopchar)    { }
   // Derived class destructor doesn't do anything base class one doesn't do.

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   char chopchar_;
   StrChunkPtrT<DataBlockStrChunk> curdata;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual void ProcessIncoming();

 private:
   CharChopper(const CharChopper &b);
};

//-----------------------------inline functions--------------------------------

inline int CharChopper::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || parentclass::AreYouA(cid));
}

#endif
