#ifndef _LCORE_WFClTypes_H_

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

#ifdef __GNUG__
#  pragma interface
#endif

#ifndef _LCORE_ClassTypes_H_
#  include <LCore/ClassTypes.h>
#endif

#ifndef _LCORE_Programmers_H_
#  include <LCore/Programmers.h>
#endif

#define _LCORE_WFClTypes_H_

namespace strmod {
namespace lcore {

class WFCORE_ClassIdent;

/** \class WF_ClassNum WFClTypes.h StrMod/WFClTypes.h
 * A ClassNum for a WinterFire Software class.  WinterFire Software no
 * longer exists, but some of its code is still around.
 */
class WF_ClassNum : public ClassNum {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;
   inline WF_ClassNum(const ClassNum &clnum);

   friend class WF_ClassIdent;

 public:
   static const WFCORE_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   enum WFCORE_Libraries { Core = 0, RecordID,
			   User0 = 4088, User1, User2, User3,
			   User4, User5, User6, User7 = 4095};

   inline U2Byte GetLibNum() const;
   inline U4Byte GetClNum() const;

   inline WF_ClassNum(WFCORE_Libraries lnum, U4Byte cnum);
};

//----------

class WFCORE_ClassIdent;

class WF_ClassIdent : public ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const WFCORE_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   const WF_ClassNum GetClass() const     { return(ClassIdent::GetClass()); }

   inline WF_ClassIdent(WF_ClassNum cnum);
};

//----------

//! Class identifier for a core library class of WinterFire software.
class WFCORE_ClassIdent : public EH_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const WFCORE_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline WFCORE_ClassIdent(U4Byte cnum);
};

//---------------------------inline functions--------------------------------

inline const ClassIdent *WF_ClassNum::i_GetIdent() const
{
   return(&identifier);
}

inline int WF_ClassNum::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ClassNum::AreYouA(cid));
}

inline U2Byte WF_ClassNum::GetLibNum() const
{
   return((0xfff00000UL & ClassNum::GetClNum()) >> 20);
}

inline U4Byte WF_ClassNum::GetClNum() const
{
   return(0x000fffffUL & ClassNum::GetClNum());
}

inline WF_ClassNum::WF_ClassNum(WFCORE_Libraries lnum, U4Byte cnum) :
     ClassNum((U4Byte(lnum & 0x0fffU) << 20) | (cnum & 0x000fffffUL))
{
}

inline WF_ClassNum::WF_ClassNum(const ClassNum &clnum) : ClassNum(clnum)
{
}

//--------------

inline const ClassIdent *WF_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int WF_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ClassIdent::AreYouA(cid));
}

inline WF_ClassIdent::WF_ClassIdent(WF_ClassNum cnum) :
   ClassIdent(EricMHopper_0, cnum)
{
}

//--------------

inline const ClassIdent *WFCORE_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int WFCORE_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || WF_ClassIdent::AreYouA(cid));
}

inline WFCORE_ClassIdent::WFCORE_ClassIdent(U4Byte cnum) :
     WF_ClassIdent(WF_ClassNum(WF_ClassNum::Core, cnum))
{
}

} // namespace lcore
} // namespace strmod

#endif
