#ifndef _STR_SimplePlug_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/08/24 12:54:58  hopper
// New source module from SimplePlug class, and SimplePlugT<T> template.
//

#include <StrMod/StreamModule.h>

#define _STR_SimplePlug_H_

// This is an abstract class that handle details of read and write
// locking on a plug.  Classes derived from this should have much
// simpler implementations.  The pure virtual i_CanWrite, i_Write,
// i_CanRead, and i_InternalRead functions shouldn't have to worry if
// a read or write is already going on.  Of course, a read might by
// happening while the write functions are being called, or
// visa-versa.

class SimplePlug : public StrPlug {
 public:
   static const STR_ClassIdent identifier;

   inline SimplePlug(StreamModule *parent);
   inline virtual ~SimplePlug()                        { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanWrite() const;
   inline virtual bool Write(const StrChunkPtr &);

   inline virtual bool CanRead() const;

   virtual int Side() const = 0;

   bool IsReading() const                              { return(readflag_); }
   bool IsWriting() const                              { return(writeflag_); }

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual const StrChunkPtr InternalRead();

   virtual bool i_CanWrite() const = 0;
   virtual bool i_Write(const StrChunkPtr &) = 0;
   virtual bool i_CanRead() const = 0;
   virtual const StrChunkPtr i_InternalRead() = 0;

   virtual void ReadableNotify();
   virtual void WriteableNotify();

 private:
   bool readflag_, writeflag_;
};

//--

// The template parameter for this class is the parent module the plug
// us from.  This handles all the ugly casting issues needed to
// implement the ModuleFrom() function.  This handles them in a
// type-safe manner.

template <class Module>
class SimplePlugOf : public SimplePlug {
 public:
   SimplePlugOf(Module *parent) : SimplePlug(parent)                        { }
   virtual ~SimplePlugOf()                                                  { }

   inline Module *ModuleFrom() const;

   virtual int Side() const = 0;

 protected:
   virtual bool i_CanWrite() const = 0;
   virtual bool i_Write(const StrChunkPtr &) = 0;
   virtual bool i_CanRead() const = 0;
   virtual const StrChunkPtr i_InternalRead() = 0;
};

//-----------------------------inline functions--------------------------------

inline SimplePlug::SimplePlug(StreamModule *parent)
     : StrPlug(parent), readflag_(false), writeflag_(false)
{
}

inline int SimplePlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrPlug::AreYouA(cid));
}

inline bool SimplePlug::CanWrite() const
{
   return(!writeflag_ && i_CanWrite());
}

inline bool SimplePlug::Write(const StrChunkPtr &c)
{
//  This is a clearer version of the uncommented code below.
//   if (writeflag_) {
//      return(false);
//   } else {
//      return(i_Write(c));
//   }
   return(!(writeflag_ || !i_Write(c)));
}

inline bool SimplePlug::CanRead() const
{
   return(!readflag_ && i_CanRead());
}

inline const StrChunkPtr SimplePlug::InternalRead()
{
   if (readflag_) {
      return(0);
   } else {
      return(i_InternalRead());
   }
}

//--

template <class Module>
inline Module *SimplePlugOf<Module>::ModuleFrom() const
{
   return(static_cast<Module *>(SimplePlug::ModuleFrom()));
}

#endif
