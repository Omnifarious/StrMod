#ifndef _STR_ApplyVisitor_H_  // -*-c++-*-

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

// For a log, see ../ChangeLog

#ifndef _STR_UseTrackingVisitor_H_
#  include <StrMod/UseTrackingVisitor.h>
#endif
#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif

#define _STR_ApplyVisitor_H_

namespace strmod {
namespace strmod {

/** \class ApplyVisitor_Base ApplyVisitor.h StrMod/ApplyVisitor.h
 * A base class for the ApplyVisitor template that factors out the common code
 * so there aren't tons of duplicates all over creation.
 *
 * This defines functions that don't vary at all no matter what type you give
 * to ApplyVisitor.  This will result in less redundant code being duplicated
 * for instances of ApplyVisitor<T> with different T.

*/
class ApplyVisitor_Base : public UseTrackingVisitor {
 public:
   /** \brief This constructor will set up apply to visit the given chunk.
    *
    * @param chunk The chunk to go through.
    */
   ApplyVisitor_Base(const StrChunkPtr &chunk);
   /** \brief This constructor will set up apply to visit the section of the
    * given chunk described by extent.
    *
    * @param chunk The chunk to go through.
    * @param extent Which part of the chunk to go through.
    */
   ApplyVisitor_Base(const StrChunkPtr &chunk, LinearExtent &extent);
   /** Whee, its a destructor, and since this class has no state that won't
    * destruct itself, it doesn't do much.
    */
   virtual ~ApplyVisitor_Base()                         { }

   /** \brief Visits every data block according to the parameters given in
    * the constructor.
   */
   void apply();

 protected:
   virtual void use_visitStrChunk(const StrChunkPtr &chunk,
                                  const LinearExtent &used)
      throw(halt_visitation)
   {
   }

   virtual void use_visitDataBlock(const void *start, size_t len,
                                   const void *realstart, size_t reallen)
      throw(halt_visitation) = 0;

 private:
   const StrChunkPtr chunk_;
   const bool extent_used_;
   const LinearExtent extent_;
};

//---

/** \class ApplyVisitor ApplyVisitor.h StrMod/ApplyVisitor.h
 * A template class that applies a functor (can be a function pointer) to each
 * chunk of bytes in a StrChunk containment hierarchy.
 *
 * The functor must accept this signature <code>any functor(const void *,
 * size_t)</code>.
 */
template <class _Function>
class ApplyVisitor : public ApplyVisitor_Base {
 public:
   /**
    * \brief Does what ApplyVisitor_Base::ApplyVisitor_Base(const StrChunkPtr
    * &) does.
    *
    * @param chunk The chunk to go through.
    * @param func The functor to run on every data extent.
    */
   inline ApplyVisitor(const StrChunkPtr &chunk, _Function &func);
   /**
    * \brief Does what ApplyVisitor_Base::ApplyVisitor_Base(const StrChunkPtr
    * &, LinearExtent &) does.
    *
    * @param chunk The chunk to go through.
    * @param extent The portion of the chunk to go through.
    * @param func The functor to run on every data extent.
    */
   inline ApplyVisitor(const StrChunkPtr &chunk, const LinearExtent &extent,
                       _Function &func);
   /** Whee, its a destructor, and since this class has no state it
    * 'owns', it doesn't do much.
    */
   inline virtual ~ApplyVisitor();

 protected:
   inline virtual void use_visitDataBlock(const void *start, size_t len,
                                          const void *realstart, size_t reallen)
      throw(halt_visitation);

 private:
   _Function &func_;
};

//-----------------------------inline functions--------------------------------

template <class _Function> inline
ApplyVisitor<_Function>::ApplyVisitor(const StrChunkPtr &chunk, _Function &func)
     : ApplyVisitor_Base(chunk), func_(func)
{
}

template <class _Function> inline
ApplyVisitor<_Function>::ApplyVisitor(const StrChunkPtr &chunk,
                                      const LinearExtent &extent,
                                      _Function &func)
     : ApplyVisitor_Base(chunk, extent), func_(func)
{
}

template <class _Function> inline
ApplyVisitor<_Function>::~ApplyVisitor()
{
}

template <class _Function> inline void
ApplyVisitor<_Function>::use_visitDataBlock(const void *start, size_t len,
                                            const void *realstart, size_t reallen)
   throw(halt_visitation)
{
   func_(start, len);
}

/** A function to hide the rather odd syntax for using the ApplyVisitor class.
 *
 * Much like the STL for_each function, but this one iterates over the data
 * extents making up a StrChunk.
 *
 * @param chunk The chunk to iterate over.
 * @param func The functor to apply.
 */
template <class _Function>
inline void for_each(const StrChunkPtr &chunk, _Function &func)
{
   ApplyVisitor<_Function> av(chunk, func);
   av.apply();
}

/** A function to hide the rather odd syntax for using the ApplyVisitor class.
 *
 * Much like the STL for_each function, but this one iterates over the data
 * extents making up the section of the StrChunk described by extent.
 *
 * @param chunk The chunk to iterate over.
 * @param extent Which LinearExtent of the chunk to iterator over.
 * @param func The functor to apply.
 */
template <class _Function>
inline void for_each(const StrChunkPtr &chunk, const LinearExtent &extent,
                     _Function &func)
{
   ApplyVisitor<_Function> av(chunk, extent, func);
   av.apply();
}

}  // namespace strmod
}  // namespace strmod

#endif
