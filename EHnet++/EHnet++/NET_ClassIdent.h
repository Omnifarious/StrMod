#ifndef _NET_NET_ClassIdent_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

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
// Revision 1.1  1996/02/12 03:01:54  hopper
// Added links to my ClassIdent system.
//

#include <LCore/HopClTypes.h>

#define _NET_NET_ClassIdent_H_

class NET_ClassIdent : public EH_ClassIdent {
 protected:
   inline virtual const ClassIdent *i_GetIdent() const;

 public:
   static const NET_ClassIdent identifier;

   inline NET_ClassIdent(U4Byte cnum);

   inline virtual int AreYouA(const ClassIdent &cid) const;
};

//---------------------------inline functions--------------------------------

inline NET_ClassIdent::NET_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::Net, cnum))
{
}

inline const ClassIdent *NET_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int NET_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

#endif
