#ifndef _UNEVT_UNEVT_ClassIdent_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For log see ../ChangeLog

#include <LCore/HopClTypes.h>
#include <LCore/Protocol.h>

#define _UNEVT_UNEVT_ClassIdent_H_

namespace strmod {
namespace unievent {

/** \class UNEVT_ClassIdent UNEVT_ClassIdent.h UniEvent/UNEVT_ClassIdent.h
 * \brief The global identifier class for classes in strmod::unievent.
 */
class UNEVT_ClassIdent : public EH_ClassIdent, virtual public Protocol {
 public:
   static const UNEVT_ClassIdent identifier;

   //! Construct a UNEVT from a class number private to strmod::unievent
   inline explicit UNEVT_ClassIdent(U4Byte cnum);

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   inline virtual const ClassIdent *i_GetIdent() const;
};

//---------------------------inline functions--------------------------------

inline const ClassIdent *UNEVT_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int UNEVT_ClassIdent::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

inline UNEVT_ClassIdent::UNEVT_ClassIdent(U4Byte cnum) :
     EH_ClassIdent(EH_ClassNum(EH_ClassNum::User3, cnum))
{
}

}; // namespace unievent
}; // namespace strmod

#endif
