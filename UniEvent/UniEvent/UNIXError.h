#ifndef _UNEVT_UNIXError_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.

#include <string>
#include <LCore/Protocol.h>
#ifndef _UNEVT_UNEVT_ClassIdent_H_
#  include <UniEvent/UNEVT_ClassIdent.h>
#endif

#define _UNEVT_UNIXError_H_

//: Holds a UNIX errno value.  Not really intended to be derived from.
// <p>There are no member functions to modify its value, so this is
// essentially a const value.  This can be useful in multi-threading
// environments because you don't have to care who else can modify your
// object.  Of course, someone CAN destroy it out from underneath...</p>
class UNIXError : virtual public Protocol {
 public:
   static const UNEVT_ClassIdent identifier;

   inline explicit UNIXError(int errno);
   inline ~UNIXError();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: The numeric value of the error.  Corresponds to global varaible errno.
   int getErrorNum() const                              { return(errno_); }
   //: The result of doing strerror(getErrorNum()).  Beware the reference return
   // <p>The reference will only be valid while the object you obtained it from
   // still exists.</p>
   const string &getErrorString() const;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

 private:
   int errno_;
   mutable bool did_strerror_;
   mutable string errstr_;

   //: Left undefined and made private on purpose.
   // <p>The copy constructor isn't here on purpose.  The default public one
   // is just fine.</p>
   void operator =(const UNIXError &val);
};

//-----------------------------inline functions--------------------------------

UNIXError::UNIXError(int errno)
     : errno_(errno), did_strerror_(false)
{
}

inline UNIXError::~UNIXError()
{
}

inline int UNIXError::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

#endif
