#ifndef _LCORE_Object_H_  // -*- c++ -*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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
// Revision 1.1.1.1  1995/07/22 04:09:25  hopper
// Imported sources
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

#include <iosfwd>

#define _LCORE_Object_H_

namespace strmod {
namespace lcore {

class ClassIdent;

class Object {
 public:
   static const ClassIdent identifier;

   Object()                                       {}
   virtual ~Object()                              {}

   const ClassIdent &GetIdent() const             { return(*i_GetIdent()); }
   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   virtual void PrintOn(std::ostream &os) const;
   inline virtual void PrintOn(std::ostream &os);
   virtual int IsEqual(const Object &b) const       { return(this == &b); }
   int DoEqual(const Object &b) const;
   inline virtual const ClassIdent *i_GetIdent() const;

 private:
   friend int operator ==(const Object &a, const Object &b);
   friend std::ostream &operator <<(std::ostream &os, const Object &ob);
   friend std::ostream &operator <<(std::ostream &os, Object &ob);

};

#define AreYouA_Template(parent)      return((identifier == cid) || \
					     parent::AreYouA(cid)); \
				   }

#define AreYouA_FuncT(parent)      return((identifier == cid) || \
					  parent::AreYouA(cid));

#ifndef _LCORE_ClassTypes_H_
#define _LCORE_Object_H_INSIDE
   #include <LCore/ClassTypes.h>
#undef _LCORE_Object_H_INSIDE
#endif

inline void Object::PrintOn(std::ostream &os)
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

inline std::ostream &operator <<(std::ostream &os, const Object &ob)
{
   ob.PrintOn(os);
   return(os);
}

inline std::ostream &operator <<(std::ostream &os, Object &ob)
{
   ob.PrintOn(os);
   return(os);
}

} // namespace lcore
} // namespace strmod

#endif
