/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "UNIXError.h"
#endif

#include "UniEvent/UNIXError.h"
#include <cstring>
#include <cstddef>

//extern "C" char *strerror_r (int __errnum, char *__buf, size_t __buflen);

namespace strmod {
namespace unievent {

const UNIXError UNIXError::S_noerror("<no error>", 0, LCoreError("<no error>"));

void UNIXError::getErrorString(char *buf, size_t buflen) const throw ()
{
   strncpy(buf, strerror(errnum_), buflen - 1);
//   strerror_r(errnum_, buf, buflen);  // Doesn't work right for now.
   buf[buflen - 1] = '\0';
}

};
};
