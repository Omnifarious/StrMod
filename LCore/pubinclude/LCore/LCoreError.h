#ifndef _LCORE_LCoreError_H_  // -*-c++-*-

/*
 * Copyright 2001-2010 Eric M. Hopper <hopper@omnifarious.org>
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

// For a log, see ../Changelog

//! Anti-double inclusion macro.
#define _LCORE_LCoreError_H_

#include <iosfwd>

namespace strmod {
namespace lcore {

/** \file LCore/LCoreError.h
 * This file defines LCORE_GET_COMPILERINFO, and contains LCoreError, and
 * LCoreError::CompilerInfo.
 */

#if defined __GNUC__ && defined __GNUC_MINOR__ && ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 6)))
/**
 * \def LCORE_GET_COMPILERINFO()
 * \brief Construct a temporary ::LCoreError::CompilerInfo object with values
 * the compiler will provide in macros.
 * This is the g++ version, and therefore has the function name available.  */
#   define LCORE_GET_COMPILERINFO() \
             ::strmod::lcore::LCoreError::CompilerInfo(__FILE__, __LINE__, __PRETTY_FUNCTION__)
# else
/** \def LCORE_GET_COMPILERINFO()
 * \brief Construct a temporary ::LCoreError::CompilerInfo object with values
 * the compiler will provide in macros.
 * This isn't the g++ version, and so has no function name.  */
#   define LCORE_GET_COMPILERINFO() \
             ::strmod::lcore::LCoreError::CompilerInfo(__FILE__, __LINE__)
#endif

/** \class LCoreError LCoreError.h LCore/LCoreError.h
 * \brief Represents an error, may be inside a thrown exception.
 *
 * This is a very simple class.  It just holds pointers to information.  It
 * expects that the information it points to will not go away.  In fact, it's
 * best if the information being pointed to consists of all constant strings
 * allocated at program start and never destroyed.
 *
 * This class is designed to be able to be embedded inside of an exception.  It
 * does no dynamic allocation, or any other operation capable of throwing an
 * exception for this reason.
 *
 * Derived classes need to be designed with the idea that there is no virtual
 * destructor.  They should also handle 'slicing' elegantly.
 *
 * Everything in this class is 'const' so that the class may also be easily used
 * in threading situations.  This also requires that everything the class points
 * at be similarily immutable.
 */
class LCoreError {
 public:
   class CompilerInfo;

   //! Contruct an LCoreError with however much information you can give it.
   explicit inline
   LCoreError(const char *desc = 0, const char *sourcefile = 0,
              unsigned int line = 0, const char *func = 0) noexcept;
   //! Contruct an LCoreError with a description, and a CompilerInfo object
   explicit inline LCoreError(const char *desc, const CompilerInfo &inf)
      noexcept;
   //! Construct an LCoreError with a CompilerInfo object, but no description.
   inline LCoreError(const CompilerInfo &inf) noexcept;

   //! Get the description, may return NULL.
   const char *getDesc() const noexcept                  { return desc_; }
   //! Get the source file, may return NULL.
   const char *getSourceFile() const noexcept            { return sourcefile_; }
   //! Get the line number.
   unsigned int getLine() const noexcept                 { return line_; }
   //! Get the function name, may return NULL.
   const char *getFunc() const noexcept                  { return func_; }

 private:
   const char * const desc_;
   const char * const sourcefile_;
   const unsigned int line_;
   const char * const func_;
};

/** \class LCoreError::CompilerInfo LCoreError.h LCore/LCoreError.h
 * \brief A class collecting together all compile-time information a compiler
 * will give it.
 *
 * This class is largely so that macros that autmatically extract the
 * information from the compiler are easier to write.  See
 * LCORE_GET_COMPILERINFO().
 */
class LCoreError::CompilerInfo
{
 public:
   //! Constructs with a source file, line number, and possible function name.
   explicit inline
   CompilerInfo(const char *sourcefile, unsigned int line, const char *func = 0)
      noexcept;

   //! Get the source file, may return NULL.
   const char *getSourceFile() const noexcept            { return sourcefile_; }
   //! Get the line number.
   unsigned int getLine() const noexcept                 { return line_; }
   //! Get the function name, may return NULL.
   const char *getFunc() const noexcept                  { return func_; }

 private:
   const char * const sourcefile_;
   const unsigned int line_;
   const char * const func_;
};

//-----------------------------inline functions--------------------------------

/**
 * \param desc A general description of the context of the error.
 * \param sourcefile The name of the file the error occured in.
 * \param line The line number the error occured on.
 * \param func The name of the function the error occured in.
 */
inline
LCoreError::LCoreError(const char *desc, const char *sourcefile,
                       unsigned int line, const char *func) noexcept
     : desc_(desc), sourcefile_(sourcefile), line_(line), func_(func)
{
}

inline
LCoreError::LCoreError(const char *desc, const CompilerInfo &inf) noexcept
     : desc_(desc),
       sourcefile_(inf.getSourceFile()), line_(inf.getLine()),
       func_(inf.getFunc())
{
}

inline LCoreError::LCoreError(const CompilerInfo &inf) noexcept
     : desc_(0),
       sourcefile_(inf.getSourceFile()), line_(inf.getLine()),
       func_(inf.getFunc())
{
}

//--

inline LCoreError::CompilerInfo::CompilerInfo(const char *sourcefile, 
                                              unsigned int line,
                                              const char *func) noexcept
     : sourcefile_(sourcefile), line_(line), func_(func)
{
}

//--

//! Print out an LCoreError on an iostream.
std::ostream &operator <<(std::ostream &os, const LCoreError &err);

} // namespace lcore
} // namespace strmod

#endif
