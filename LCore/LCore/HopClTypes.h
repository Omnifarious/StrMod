#ifndef _LCORE_HopClTypes_H_

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

// For log, see ../ChangeLog
//
// Revision 1.5  1998/05/01 11:59:32  hopper
// Changed bool to bool_val or bool_cst as appropriate so it will be easier to
// port to platforms that don't support bool.
//
// Revision 1.4  1996/10/01 19:17:26  hopper
// Added Trashcan library to libraries list.
//
// Revision 1.3  1996/03/26 16:23:00  hopper
// Moved ReferenceCounting class over from Container library.
// Fixed up class numbering system so LCore uses a seperate library number
// from the container library.
//
// Revision 1.2  1996/02/12 00:43:48  hopper
// Added Net library class EH0_Libraries enum in class EH_ClassNum.
//
// Revision 1.1.1.1  1995/07/22 04:09:25  hopper
// Imported sources
//
// Revision 0.4  1995/03/08 05:31:50  hopper
// Added StrMod library to EH0_Libraries enum
//
// Revision 0.3  1994/10/30  04:40:44  hopper
// Moved various things into the new LCore library.
//
// Revision 0.2  1994/09/12  03:31:27  hopper
// Added the RecordID constant to the EH0_Libraries enum.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
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

#define _LCORE_HopClTypes_H_

namespace strmod {
namespace lcore {

class LCore_ClassIdent;

class EH_ClassNum : public ClassNum {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;
   inline EH_ClassNum(const ClassNum &clnum);

   friend class EH_ClassIdent;

 public:
   static const LCore_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   enum EH0_Libraries { Container = 0, Common, ETurboVis, Accounting,
			FileIO, RecordID, StrMod, Net, LCore, ADT,
			Logger = 4083, MO_Entry, DriverFile, Journals, GMCs,
			User0, User1, User2, User3,
			User4, User5, User6, User7 = 4095};

   inline U2Byte GetLibNum() const;
   inline U4Byte GetClNum() const;

   inline EH_ClassNum(EH0_Libraries lnum, U4Byte cnum);
};

//----------

class EH_ClassIdent : public ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const LCore_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   const EH_ClassNum GetClass() const     { return(ClassIdent::GetClass()); }

   inline EH_ClassIdent(EH_ClassNum cnum);
};

//----------

class LCore_ClassIdent : public EH_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const LCore_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline LCore_ClassIdent(U4Byte cnum);
};

//---------------------------inline functions--------------------------------

inline const ClassIdent *EH_ClassNum::i_GetIdent() const
{
   return(&identifier);
}

inline int EH_ClassNum::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ClassNum::AreYouA(cid));
}

inline U2Byte EH_ClassNum::GetLibNum() const
{
   return((0xfff00000UL & ClassNum::GetClNum()) >> 20);
}

inline U4Byte EH_ClassNum::GetClNum() const
{
   return(0x000fffffUL & ClassNum::GetClNum());
}

inline EH_ClassNum::EH_ClassNum(EH0_Libraries lnum, U4Byte cnum) :
     ClassNum((U4Byte(lnum & 0x0fffU) << 20) | (cnum & 0x000fffffUL))
{
}

inline EH_ClassNum::EH_ClassNum(const ClassNum &clnum) : ClassNum(clnum)
{
}

//--------------

inline const ClassIdent *EH_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int EH_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ClassIdent::AreYouA(cid));
}

inline EH_ClassIdent::EH_ClassIdent(EH_ClassNum cnum) :
   ClassIdent(EricMHopper_0, cnum)
{
}

//--------------

inline const ClassIdent *LCore_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int LCore_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

inline LCore_ClassIdent::LCore_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::LCore, cnum))
{
}

} // namespace lcore
} // namespace strmod

#endif
