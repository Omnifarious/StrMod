#ifndef _STR_FDUtil_H_  // -*-c++-*-

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
