#ifndef _STR_STR_ClassIdent_H_

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

// For a log, see ../ChangeLog
//
// Revision 1.3  1999/03/05 15:30:32  hopper
// Some lint cleanup changes, and also cleanups and fixes to the code
// that handles interplug communication and notification.
//
// Revision 1.2  1996/02/12 05:50:04  hopper
// Added operator == to work around g++ 2.7.2 bug.
//
// Revision 1.1.1.1  1995/07/22 04:46:51  hopper
// Imported sources
//
// Revision 1.2  1995/04/05  04:52:07  hopper
// Fixed a stupid mistake.
//
// Revision 1.1  1995/03/08  05:34:44  hopper
// Initial revision
//

#include <LCore/HopClTypes.h>

#define _STR_STR_ClassIdent_H_

namespace strmod {
namespace strmod {

/** \class STR_ClassIdent STR_ClassIdent.h StrMod/STR_ClassIdent.h
 * The class identifier class that sets the library number section of the
 * class number to be correct for the classes in the StrMod library.
 */
class STR_ClassIdent : public lcore::EH_ClassIdent {
   typedef lcore::U4Byte U4Byte;
 public:
   //! The unique class identifier for this class.
   static const STR_ClassIdent identifier;

   /** Construct a class identifier.
    * This class the base class constructor with the appropriate library
    * number.
    */
   inline STR_ClassIdent(U4Byte cnum);

   /**
    * Asks if a class is of a particular type, or publicly derived from that
    * type.
    *
    * Overriden in every derived class (with a static identifier member) to
    * compare against the identifier, then call the AreYouA methods of all the
    * superclasses.
    *
    * @param cid Usually <class>::identifier for the class you want to ask if
    * the object is an instance of.
    */
   inline virtual int AreYouA(const ClassIdent &cid) const;

//     inline bool operator ==(const ClassIdent &b) const;

 protected:
   /**
    * Returns the class identifier for the class the object actually is.
    *
    * Should <strong>always</strong> be overridden in any class that has a
    * static identifier member.
    */
   inline virtual const ClassIdent *i_GetIdent() const;
};

//---------------------------inline functions--------------------------------

inline const lcore::ClassIdent *STR_ClassIdent::i_GetIdent() const
{
   return(&identifier);
}

inline int STR_ClassIdent::AreYouA(const lcore::ClassIdent &cid) const
{
   return((identifier == cid) || EH_ClassIdent::AreYouA(cid));
}

//  inline bool STR_ClassIdent::operator ==(const ClassIdent &b) const
//  {
//     return(EH_ClassIdent::operator ==(b));
//  }

inline STR_ClassIdent::STR_ClassIdent(U4Byte cnum) :
     lcore::EH_ClassIdent(lcore::EH_ClassNum(lcore::EH_ClassNum::StrMod, cnum))
{
}

}  // namespace strmod
}  // namespace strmod

#endif
