#ifndef _LCORE_LCoreError_H_  // -*-c++-*-

/*
 * Copyright (C) 2001 Eric M. Hopper <hopper@omnifarious.mn.org>
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

// For a log, see ../Changelog

#define _LCORE_LCoreError_H_

#if defined __GNUC__ && defined __GNUC_MINOR__ && ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 6)))
#   define LCORE_GET_COMPILERINFO() \
             ::LCoreError::CompilerInfo(__FILE__, __LINE__, __PRETTY_FUNCTION__)
# else
#   define LCORE_GET_COMPILERINFO(desc) \
             ::LCoreError::CompilerInfo(__FILE__, __LINE__)
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

   explicit inline
   LCoreError(const char *desc = 0, const char *sourcefile = 0,
              unsigned int line = 0, const char *func = 0) throw ();
   explicit inline LCoreError(const char *desc, const CompilerInfo &inf)
      throw();
   inline LCoreError(const CompilerInfo &inf) throw();

   const char *getDesc() const throw ()                  { return desc_; }
   const char *getSourceFile() const throw ()            { return sourcefile_; }
   unsigned int getLine() const throw ()                 { return line_; }
   const char *getFunc() const throw ()                  { return func_; }

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
 * LCORE_GET_COMPILERINFO.
 */
class LCoreError::CompilerInfo
{
 public:
   explicit inline
   CompilerInfo(const char *sourcefile, unsigned int line, const char *func = 0)
      throw();

   const char *getSourceFile() const throw()             { return sourcefile_; }
   unsigned int getLine() const throw ()                 { return line_; }
   const char *getFunc() const throw ()                  { return func_; }

 private:
   const char * const sourcefile_;
   const unsigned int line_;
   const char * const func_;
};

//-----------------------------inline functions--------------------------------

inline
LCoreError::LCoreError(const char *desc = 0, const char *sourcefile = 0,
                       unsigned int line = 0, const char *func = 0) throw ()
     : desc_(desc), sourcefile_(sourcefile), line_(line), func_(func)
{
}

inline
LCoreError::LCoreError(const char *desc, const CompilerInfo &inf) throw()
     : desc_(desc),
       sourcefile_(inf.getSourceFile()), line_(inf.getLine()),
       func_(inf.getFunc())
{
}

inline LCoreError::LCoreError(const CompilerInfo &inf) throw()
     : desc_(0),
       sourcefile_(inf.getSourceFile()), line_(inf.getLine()),
       func_(inf.getFunc())
{
}

//--

inline LCoreError::CompilerInfo::CompilerInfo(const char *sourcefile, 
                                              unsigned int line,
                                              const char *func = 0) throw()
     : sourcefile_(sourcefile), line_(line), func_(func)
{
}

#endif
