#ifndef _LCORE_Protocol_H_

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
// Revision 1.2  1996/03/26 16:23:01  hopper
// Moved ReferenceCounting class over from Container library.
// Fixed up class numbering system so LCore uses a seperate library number
// from the container library.
//
// Revision 1.1.1.1  1995/07/22 04:09:25  hopper
// Imported sources
//
// Revision 0.2  1994/10/30  04:41:41  hopper
// Moved various things into the new LCore library.
//
// Revision 0.1  1994/07/21  05:38:24  hopper
// Genesis!
//

#ifdef __GNUG__
#  pragma interface
#endif

#ifndef _LCORE_HopClTypes_H_
#  include <LCore/HopClTypes.h>
#endif

#define _LCORE_Protocol_H_

namespace strmod {
namespace lcore {

/**
 * \class Protocol Protocol.h LCore/Protocol.h
 * A base class for my own type identification system.
 *
 * This type identification system uses IDs that are assigned to a class by a
 * programmer so they remain constant.  This means you can use the IDs for
 * type identification of data in persistent storage.
 */
class Protocol {
 public:
   static const LCore_ClassIdent identifier;

   /**
    * Gets the identifier for the class the object actually is.
    *
    * This is an interface mistake I'll have to live with until I get the
    * gumption to make the pervasive changes to correct it.
    *
    * Originally the plan was to have this return different types (derived
    * from ClassIdent) in different classes.  After I tried it a couple of
    * times, I realized what a bad idea that was, and am now left with this
    * interface.
    *
    * In order to achieve the original goal, a function that was virtual was
    * needed to return that actual identifier, and a non-virtual function was
    * needed to do the possible cast.  I decided to have just one interface
    * for getting the ClassIdent, so I made the virtual function protected.
    * That's why this looks like it does.
    */
   const ClassIdent &GetIdent() const               { return(*i_GetIdent()); }
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
   virtual int AreYouA(const ClassIdent &cid) const {
      return(identifier == cid);
   }

   //! Do nothing constructor for interface class with no member variables.
   Protocol()                                       {}
   //! Do nothing (virtual) destructor for interface class with no member
   //! variables.
   virtual ~Protocol()                              {}

 protected:
   /**
    * Returns the class identifier for the class the object actually is.
    *
    * Should <strong>always</strong> be overridden in any class that has a
    * static identifier member.
    */
   inline virtual const ClassIdent *i_GetIdent() const;
};

inline const ClassIdent *Protocol::i_GetIdent() const
{
   return(&identifier);
}

} // namespace lcore
} // namespace strmod

#endif
