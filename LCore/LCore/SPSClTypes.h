#ifndef _LCORE_SPSClTypes_H_  // -*-c++-*-

/* $Header$ */

// $Log$
// Revision 1.6  1999/01/12 04:01:46  hopper
// All kinds of changes to make the code more portable and compile
// properly with the DEC compiler.
//
// Revision 1.5  1998/05/01 11:59:33  hopper
// Changed bool to bool_val or bool_cst as appropriate so it will be easier to
// port to platforms that don't support bool.
//
// Revision 1.4  1996/07/08 02:29:05  hopper
// Added EDIServ application to class library list.
//
// Revision 1.3  1996/06/18 03:33:16  hopper
// Added comment so emacs would know this is a C++ header.
//
// Revision 1.2  1996/02/20 01:34:27  hopper
// Added explicit declarations for some operator == that would normally be
// matched by overloading rules if it weren't for a gcc 2.7.2 bug.
//
// Revision 1.1.1.1  1995/07/22 04:09:25  hopper
// Imported sources
//

#ifdef __GNUG__
#  pragma interface
#endif

#include <bool.h>

#ifndef _LCORE_ClassTypes_H_
#  include <LCore/ClassTypes.h>
#endif

#ifndef _LCORE_Programmers_H_
#  include <LCore/Programmers.h>
#endif

#define _LCORE_SPSClTypes_H_

class SPSCORE_ClassIdent;

class SPS_ClassNum : public ClassNum {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;
   inline SPS_ClassNum(const ClassNum &clnum);

   friend class SPS_ClassIdent;

 public:
   static const SPSCORE_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   enum SPSCORE_Libraries { Core = 0, Database,
			    EDIServ = 4086, Mail = 4087,
			    User0, User1, User2, User3,
			    User4, User5, User6, User7 = 4095};

   inline U2Byte GetLibNum() const;
   inline U4Byte GetClNum() const;

   inline SPS_ClassNum(SPSCORE_Libraries lnum, U4Byte cnum);
};

//----------

class SPSCORE_ClassIdent;

class SPS_ClassIdent : public ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const SPSCORE_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   const SPS_ClassNum GetClass() const     { return(ClassIdent::GetClass()); }

   inline bool_val operator ==(const ClassIdent &b) const;

   inline SPS_ClassIdent(SPS_ClassNum cnum);
};

//----------

class SPSCORE_ClassIdent : public SPS_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const SPSCORE_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline bool_val operator ==(const ClassIdent &b) const;

   inline SPSCORE_ClassIdent(U4Byte cnum);
};

//---------------------------inline functions--------------------------------

inline const ClassIdent *SPS_ClassNum::i_GetIdent() const
{
   return(&identifier);
}

inline int SPS_ClassNum::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ClassNum::AreYouA(cid));
}

inline U2Byte SPS_ClassNum::GetLibNum() const
{
   return((0xfff00000UL & ClassNum::GetClNum()) >> 20);
}

inline U4Byte SPS_ClassNum::GetClNum() const
{
   return(0x000fffffUL & ClassNum::GetClNum());
}

inline SPS_ClassNum::SPS_ClassNum(SPSCORE_Libraries lnum, U4Byte cnum) :
     ClassNum((U4Byte(lnum & 0x0fffU) << 20) | (cnum & 0x000fffffUL))
{
}

inline SPS_ClassNum::SPS_ClassNum(const ClassNum &clnum) : ClassNum(clnum)
{
}

//--------------

inline const ClassIdent *SPS_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int SPS_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ClassIdent::AreYouA(cid));
}

inline bool_val SPS_ClassIdent::operator ==(const ClassIdent &b) const
{
   return(ClassIdent::operator ==(b));
}

inline SPS_ClassIdent::SPS_ClassIdent(SPS_ClassNum cnum) :
   ClassIdent(StPaulSoftware_0, cnum)
{
}

//--------------

inline const ClassIdent *SPSCORE_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int SPSCORE_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || SPS_ClassIdent::AreYouA(cid));
}

inline bool_val SPSCORE_ClassIdent::operator ==(const ClassIdent &b) const
{
   return(SPS_ClassIdent::operator ==(b));
}

inline SPSCORE_ClassIdent::SPSCORE_ClassIdent(U4Byte cnum) :
     SPS_ClassIdent(SPS_ClassNum(SPS_ClassNum::Core, cnum))
{
}

#endif
