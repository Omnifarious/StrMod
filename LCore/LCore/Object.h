#ifndef _LCORE_Object_H_

/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:09:25  hopper
// Initial revision
//
// Revision 0.2  1994/10/30  04:41:00  hopper
// Moved various things into the new LCore library.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#ifdef __GNUG__
#  pragma interface
#endif

#define _LCORE_Object_H_

class ClassIdent;
class ostream;

class Object {
   friend int operator ==(const Object &a, const Object &b);
   friend ostream &operator <<(ostream &os, const Object &ob);
   friend ostream &operator <<(ostream &os, Object &ob);

 protected:
   virtual void PrintOn(ostream &os) const;
   inline virtual void PrintOn(ostream &os);
   virtual int IsEqual(const Object &b) const       { return(this == &b); }
   int DoEqual(const Object &b) const;
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const ClassIdent identifier;

   const ClassIdent &GetIdent() const             { return(*i_GetIdent()); }
   inline virtual int AreYouA(const ClassIdent &cid) const;

   Object()                                       {}
   virtual ~Object()                              {}
};

#define AreYouA_Template(parent)      return((identifier == cid) || \
					     parent::AreYouA(cid)); \
				   }

#define AreYouA_FuncT(parent)      return((identifier == cid) || \
					  parent::AreYouA(cid));

#ifndef _LCORE_ClassTypes_H_
   #include <LCore/ClassTypes.h>
#endif

inline void Object::PrintOn(ostream &os)
{
   ((const Object *)(this))->PrintOn(os);
}

inline const ClassIdent *Object::i_GetIdent() const
{
   return(&identifier);
}

inline int Object::AreYouA(const ClassIdent &cid) const
{
   return(cid == identifier);
}

inline int operator ==(const Object &a, const Object &b)
{
   return((&a == &b) || a.DoEqual(b));
}

inline int operator !=(const Object &a, const Object &b)
{
   return (!(a == b));
}

inline ostream &operator <<(ostream &os, const Object &ob)
{
   ob.PrintOn(os);
   return(os);
}

inline ostream &operator <<(ostream &os, Object &ob)
{
   ob.PrintOn(os);
   return(os);
}

#endif
