#ifndef _UNEVT_UNIXError_H_  // -*-c++-*-

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

// See ../ChangeLog for log.

#include <LCore/LCoreError.h>
#include <cstddef>  // For size_t
#include <cerrno>  // For errno

#define _UNEVT_UNIXError_H_

/** \class UNIXError UNIXError.h UniEvent/UNIXError.h
 * \brief Holds a UNIX errno value.
 *
 * There are no member functions to modify its value, so this is essentially a
 * const value.  This makes the class multithread safe.
 */
class UNIXError : public LCoreError {
 public:
   /** \brief Create using a system call name, the global 'errno' value, and the
    * LCORE_GET_COMPILERINFO macro to give values to parent.
    */
   explicit inline UNIXError(const char *syscallname, const char *desc = 0)
      throw ();
   /** \brief Create using a system call name, an errno value, and the
    * LCORE_GET_COMPILERINFO macro to give values to parent.
    */
   inline UNIXError(const char *syscallname, int errnum, const char *desc = 0)
      throw();
   /** Create from a system call name, the global 'errno' value, and an
    * LCoreError.
    */
   inline UNIXError(const char *syscallname, LCoreError &lcerr) throw();
   //! Create from a system call name, an errno value, and an LCoreError.
   inline UNIXError(const char *syscallname, int errnum, LCoreError &lcerr)
      throw();

   const char *getSyscallName() const throw ()          { return syscallname_; }

   //! The numeric value of the error, which corresponds to values from errno.h
   int getErrorNum() const throw ()                     { return errnum_; }
   /** The result of doing strerror(getErrorNum()).
    *
    * \param buf A buffer to stuff the string into.
    * \param buflen The maximum number of bytes the buffer can hold.
    *
    * <code>buf[buflen - 1]</code> will always be set to '\0'.
    *
    * One some platforms (like Linux), strerror is not thread safe, and a
    * different function, strerror_r is called.  strerror_r requires you to
    * supply a buffer.  Since the strerror_r interface can be implemented in
    * terms of the strerror interface, but not the other way around, the
    * strerror_r interface is what is supported here.
    */
   void getErrorString(char *buf, size_t buflen) const throw ();

 private:
   const char *syscallname_;
   const int errnum_;
};

//-----------------------------inline functions--------------------------------

inline
UNIXError::UNIXError(const char *syscallname, const char *desc) throw ()
     : LCoreError(desc, LCORE_GET_COMPILERINFO()),
       syscallname_(syscallname), errnum_(errno)
{
}

inline
UNIXError::UNIXError(const char *syscallname, int errnum, const char *desc)
   throw ()
     : LCoreError(desc, LCORE_GET_COMPILERINFO()),
       syscallname_(syscallname), errnum_(errnum)
{
}

inline UNIXError::UNIXError(const char *syscallname, LCoreError &lcerr) throw()
     : LCoreError(lcerr), syscallname_(syscallname), errnum_(errno)
{
}

inline
UNIXError::UNIXError(const char *syscallname, int errnum, LCoreError &lcerr)
   throw ()
     : LCoreError(lcerr), syscallname_(syscallname), errnum_(errnum)
{
}

#endif
