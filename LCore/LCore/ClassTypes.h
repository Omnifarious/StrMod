#ifndef _LCORE_ClassTypes_H_

/* $Header$ */

// For a log, see ../ChangeLog
//
// Revision 1.1.1.1  1995/07/22 04:09:24  hopper
// Imported sources
//
// Revision 0.2  1994/10/30 04:40:13  hopper
// Moved various things into the new LCore library.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#ifdef __GNUG__
#  pragma interface
#endif

#ifndef _LCORE_Object_H_
#  include <LCore/Object.h>
#else

#define _LCORE_ClassTypes_H_

#ifndef _LCORE_GenTypes_H_
#  include <LCore/GenTypes.h>
#endif

class ClassIdent;

class ProgrammerNum : public Object {
   U4Byte num;

 protected:
   inline virtual int IsEqual(const Object &b) const;
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   U4Byte GetPrNum() const                        { return(num); }
   friend inline bool operator ==(const ProgrammerNum &a,
				  const ProgrammerNum &b);

   ProgrammerNum(U4Byte n) : num(n)               {}
};

class ClassNum : public Object {
   U4Byte num;

 protected:
   inline virtual int IsEqual(const Object &b) const;
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   U4Byte GetClNum() const                     { return(num); }
   friend inline bool operator ==(const ClassNum &a, const ClassNum &b);

   ClassNum(U4Byte n) : num(n)                 {}
};


class ClassIdent : public Object {
   ProgrammerNum prnum;
   ClassNum clnum;

 protected:
   virtual void PrintOn(ostream &os) const;
   virtual void PrintOn(ostream &os)                  { Object::PrintOn(os); }
   inline virtual int IsEqual(const Object &b) const;
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   const ProgrammerNum &GetProgrammer() const         { return(prnum); }
   const ClassNum &GetClass() const                   { return(clnum); }
   friend inline bool operator ==(const ClassIdent &a, const ClassIdent &b);

   inline ClassIdent(const ProgrammerNum &pn, const ClassNum &cn);
};

//------------------inline functions for class ProgrammerNum----------------

inline bool operator ==(const ProgrammerNum &a, const ProgrammerNum &b)
{
   return(a.num == b.num);
}

inline int ProgrammerNum::IsEqual(const Object &b) const
{
   const ProgrammerNum *pnp = (const ProgrammerNum *)(&b);

   return(*this == *pnp);
}

inline const ClassIdent *ProgrammerNum::i_GetIdent() const
{
   return(&identifier);
}

inline int ProgrammerNum::AreYouA(const ClassIdent &cid) const
{
   return ((identifier == cid) || Object::AreYouA(cid));
}

//------------------inline functions for class ClassNum----------------------

inline bool operator ==(const ClassNum &a, const ClassNum &b)
{
   return(a.num == b.num);
}

inline int ClassNum::IsEqual(const Object &b) const
{
   const ClassNum *pnp = (const ClassNum *)(&b);

   return(*this == *pnp);
}

inline const ClassIdent *ClassNum::i_GetIdent() const
{
   return(&identifier);
}

inline int ClassNum::AreYouA(const ClassIdent &cid) const
{
   return ((identifier == cid) || Object::AreYouA(cid));
}

//-----------------inline functions for class ClassIdent---------------------

inline bool operator ==(const ClassIdent &a, const ClassIdent &b)
{
   return((&a == &b) || ((a.clnum == b.clnum) && (a.prnum == b.prnum)));
}

inline int ClassIdent::IsEqual(const Object &b) const
{
   const ClassIdent *pnp = (const ClassIdent *)(&b);

   return(*this == *pnp);
}

inline const ClassIdent *ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline ClassIdent::ClassIdent(const ProgrammerNum &pn, const ClassNum &cn) :
     prnum(pn), clnum(cn)
{
}

inline int ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return ((identifier == cid) || Object::AreYouA(cid));
}

#endif
#endif

