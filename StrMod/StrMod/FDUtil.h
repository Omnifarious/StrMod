#ifndef _STR_FDUtil_H_  // -*-c++-*-

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

// $Revision$

#define _STR_FDUtil_H_

//: A class utility library for file descriptor operations.
class FDUtil {
 public:
   //: Set a file descriptor to POSIX non-blocking.
   // This will turn off O_NDELAY and turn on O_NONBLOCK.
   // Returns true on success, false on failure.  If there's a failure,
   // myerrno will be set to the errno result that precipitated the failure,
   // otherwise, it will not be touched.
   // fd is an IN parameter, and myerrno is an OUT parameter.
   static bool setNonBlock(int fd, int &myerrno);

   //: Set a file descriptor to blocking.
   // Removes O_NDELAY and O_NONBLOCK.
   // Returns true on success, false on failure.  If there's a failure,
   // myerrno will be set to the errno result that precipitated the failure,
   // otherwise, it will not be touched.
   // fd is an IN parameter, and myerrno is an OUT parameter.
   static bool setBlock(int fd, int &myerrno);
};

#endif
