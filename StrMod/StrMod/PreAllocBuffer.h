#ifndef _STR_PreAllocBuffer_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/BufferChunk.h>

#define _STR_PreAllocBuffer_H_

//: Just a base class that defines the non-varying functions for the template
//: class.
class PreAllocBufferBase : public BufferChunk {
 private:
   void a_silly_member_function_to_make_sure_a_vtable_is_generated();
 public:
   static const STR_ClassIdent identifier;

   PreAllocBufferBase()                                 { }
   virtual ~PreAllocBufferBase() = 0;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   //: See class Debugable
   virtual bool invariant() const = 0;

   //: See class Debugable
   virtual void printState(ostream &os) const = 0;

   virtual void resize(unsigned int newsize) throw(bad_alloc) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   //: See the base class StrChunk.
   // <p>Do nothing because there's no really good way to drop parts of a
   // buffer.</p>
   virtual void i_DropUnused(const LinearExtent &usedextent,
			     KeepDir keepdir)           { }

   void i_destruct(const U1Byte * const preallocbuf);
   void i_resize(const unsigned int newsize,
		 const unsigned int prebufsize,
		 U1Byte * const preallocbuf) throw(bad_alloc);
   bool i_invariant(const unsigned int prebufsize,
		    const void * const prebuf) const;
   void i_printState(ostream &os,
		     const unsigned int prebufsize,
		     const void * const prebuf) const;
};

//---

//: A template class for buffers that contain a certain fixed amount of
//: storage that's not dynamically allocated.
template <unsigned int TInitialAlloc>
class PreAllocBuffer : public PreAllocBufferBase {
 public:
   // There isn't any identifier here because there's no good way (in the
   // identifier system) to generate a unique identifier for every template
   // instantiation.
   inline PreAllocBuffer();
   inline virtual ~PreAllocBuffer();

   inline virtual bool invariant() const;

   inline virtual void printState(ostream &os) const;

   inline virtual void resize(unsigned int newsize) throw(bad_alloc);

 private:
   U1Byte preallocbuf_[TInitialAlloc];
};

//-----------------------------inline functions--------------------------------

inline int PreAllocBufferBase::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || BufferChunk::AreYouA(cid));
}

//---

template <unsigned int TInitialAlloc>
inline PreAllocBuffer<TInitialAlloc>::PreAllocBuffer()
{
}

template <unsigned int TInitialAlloc>
inline PreAllocBuffer<TInitialAlloc>::~PreAllocBuffer()
{
   i_destruct(preallocbuf_);
}

template <unsigned int TInitialAlloc>
inline bool PreAllocBuffer<TInitialAlloc>::invariant() const
{
   return(i_invariant(TInitialAlloc, preallocbuf_));
}

template <unsigned int TInitialAlloc>
inline void PreAllocBuffer<TInitialAlloc>::printState(ostream &os) const
{
   return(i_printState(os, TInitialAlloc, preallocbuf_));
}

template <unsigned int TInitialAlloc>
inline void
PreAllocBuffer<TInitialAlloc>::resize(unsigned int newsize) throw(bad_alloc)
{
   i_resize(newsize, TInitialAlloc, preallocbuf_);
}

#endif
