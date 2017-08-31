#ifndef _UNEVT_UNIXError_H_  // -*-c++-*-

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

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.

#include <LCore/LCoreError.h>
#include <cstddef>  // For size_t
#include <cerrno>  // For errno

#define _UNEVT_UNIXError_H_

namespace strmod {
namespace unievent {

/** \class UNIXError UNIXError.h UniEvent/UNIXError.h
 * \brief Holds a UNIX errno value.
 *
 * There are no member functions to modify its value, so this is essentially a
 * const value.  This makes the class multithread safe.
 *
 * There is a special kind of error called an EOF error.  An EOF error signals
 * that an EOF condition exists on the file descriptor in questoin.  The 'errno'
 * value is ESUCCESS (0) in this case.
 */
class UNIXError : public lcore::LCoreError
{
 private:
   struct junk;
 public:
   //! A value for no error at all.
   static const UNIXError S_noerror;

   //! Create an EOF error from a system call name, and an LCoreError.
   inline UNIXError(const char *syscallname,
                    const lcore::LCoreError &lcerr) noexcept;
   //! Create from a system call name, an errno value, and an LCoreError.
   inline UNIXError(const char *syscallname, int errnum,
                    const lcore::LCoreError &lcerr) noexcept;

   //! Return name of system call that caused error.
   const char *getSyscallName() const noexcept          { return syscallname_; }

   //! The numeric value of the error, which corresponds to values from errno.h
   int getErrorNum() const noexcept                     { return errnum_; }
   /** The result of doing <code>strerror_r(getErrorNum(), buf, buflen)</code>.
    *
    * \param buf A buffer to stuff the string into.
    * \param buflen The maximum number of bytes the buffer can hold.
    *
    * <code>buf[buflen - 1]</code> will always be set to '\\0'.
    *
    * One some platforms (like Linux), strerror is not thread safe, and a
    * different function, strerror_r is called.  strerror_r requires you to
    * supply a buffer.  The sterror_r interface is supported since the
    * strerror_r interface can be implemented in terms of the strerror
    * interface, but not the other way around.
    */
   void getErrorString(char *buf, size_t buflen) const noexcept;

   //! Is this an End-Of-File error?
   bool isEOF() const noexcept                          { return iseof_; }

   static int getErrno()                                { return errno; }

 private:
   const char *syscallname_;
   const int errnum_;
   const bool iseof_;

   struct junk {
   };
};

//-----------------------------inline functions--------------------------------

/**
 * \param syscallname Name of system call that generated the error.
 * \param lcerr An LCoreError, it's values will be copied into the UNIXError.
 * The actual Unix error number is grabbed from the global 'errno' value.
 */
inline
UNIXError::UNIXError(const char *syscallname,
                     const lcore::LCoreError &lcerr) noexcept
     : LCoreError(lcerr), syscallname_(syscallname),
       errnum_(0), iseof_(true)
{
}

/**
 * \param syscallname Name of system call that generated the error.
 * \param errnum The Unix 'errno' value for this error.
 * \param lcerr An LCoreError, it's values will be copied into the UNIXError.
 */
inline
UNIXError::UNIXError(const char *syscallname,
                     int errnum, const lcore::LCoreError &lcerr)
     noexcept
     : LCoreError(lcerr), syscallname_(syscallname),
       errnum_(errnum), iseof_(false)
{
}

} // namespace unievent
} // namespace strmod

#endif
