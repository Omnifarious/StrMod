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

// For log see ChangeLog

#ifdef __GNUG__
#  pragma implementation "UNEVT_ClassIdent.h"
#endif

#include "UniEvent/UNEVT_ClassIdent.h"

namespace strmod {
/** A generic, 'unified' event system.
 * This namespace is used for a set of classes implementing a fairly generic
 * event subsystem.  It also has some OS specific classes for capturing OS
 * events and bring them into the generic event subsystem.
 */
namespace unievent {

/** \class UNEVT_ClassIdent UNEVT_ClassIdent.h UniEvent/UNEVT_ClassIdent.h
 * \brief An ignored blurb.
 *
 * Use of the ClassIdent classes is being phased out in favor of RTTI.
 *
 * Here is a list of currently used identifiers:
 * <pre>
 *   0UL  UNEVT_ClassIdent
 *   1UL  Dispatcher
 *   2UL  Event
 *   3UL  OSConditionManager
 *   4UL  SimpleDispatcher
 *   5UL  EventPtr
 *   6UL  UNIXSignalHandler
 *   7UL  UNIXpollManager
 *   8UL  UNIXpollManager::PollEvent
 *   9UL  UNIXpollManagerImp
 *  10UL  <nothing, was UNIXError>
 *  11UL  UNIXSignalHandler
 *  12UL  Timer
 *  13UL  UNIXTimer
 * </pre>
 */

const UNEVT_ClassIdent UNEVT_ClassIdent::identifier(0UL);

};
};
