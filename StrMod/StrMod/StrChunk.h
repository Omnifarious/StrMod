#ifndef _STR_StrChunk_H_  // -*-c++-*-

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// See ../ChangeLog for log.
// Revision 1.5  1996/06/29 06:27:12  hopper
// Completely re-worked for new StrChunk handling.
//
// Revision 1.4  1996/02/26 03:42:15  hopper
// Added stuff for new ShallowCopy method.
//
// Revision 1.3  1996/02/22 03:52:28  hopper
// Made some changes now that we have ReferenceCounting again.
//
// Revision 1.2  1996/02/12 05:50:49  hopper
// Declared something to be inline that should be inline.
//
// Revision 1.1.1.1  1995/07/22 04:46:50  hopper
// Imported sources
//
// ->Revision 0.28  1995/04/05  04:52:22  hopper
// ->Fixed a stupid mistake.
// ->
// ->Revision 0.27  1995/04/05  03:11:36  hopper
// ->Changed things for integration into the rest of my libraries.
// ->
// ->Revision 0.26  1995/01/13  17:32:57  hopper
// ->Merged versions 0.25, and 0.25.0.8 back together.
// ->
// ->Revision 0.25.0.8  1995/01/13  17:27:54  hopper
// ->Made the #include directives work under OS/2.
// ->
// ->Revision 0.25.0.7  1994/07/18  04:01:03  hopper
// ->Fixed syntax errors that gcc 2.6.0 just found...
// ->Major syntax/semantic change: Uses unsigned char's for everything
// ->now.
// ->
// ->Revision 0.25.0.6  1994/06/20  04:23:25  hopper
// ->Added Resize(unsigned int) method to DataBlockStrChunk.
// ->
// ->Revision 0.25.0.5  1994/06/16  02:19:18  hopper
// ->Added #pragma interface declaration.
// ->
// ->Revision 0.25.0.4  1994/06/08  05:13:09  hopper
// ->Fixed some stupid minor syntax errors/typos.
// ->
// ->Revision 0.25.0.3  1994/06/08  04:55:28  hopper
// ->Added better formatting. Added stuff to support using a different I/O form
// ->for sockets. This will probably only be used by OS2SocketModule.
// ->
// ->Revision 0.25.0.2  1994/05/16  05:21:37  hopper
// ->Removed dependency on my ReferenceCounting class.
// ->
// ->Revision 0.25.0.1  1994/05/16  02:42:46  hopper
// ->No changes, but I needed a head for the inevitable WinterFire-OS/2
// ->branch.
// ->
// ->Revision 0.25  1994/05/07  03:40:55  hopper
// ->Move enclosing #ifndef ... #define bits around. Changed names of some
// ->non-multiple include constants. Changed directories for certain
// ->type of include files.
// ->
// ->Revision 0.24  1994/04/14  15:41:22  hopper
// ->More minor changes.
// ->
// ->Revision 0.23  1994/04/14  15:40:33  hopper
// ->Minor, minor change.
// ->
// ->Revision 0.22  1992/05/28  19:29:03  hopper
// ->Fixed a few stupid syntax errors. They involved not having inline
// ->in the right place.
// ->
// ->Revision 0.21  1992/05/22  03:26:48  hopper
// ->Added some constructors to DataBlockStrChunk that should've
// ->been there already.
// ->
// ->Revision 0.20  1992/04/15  02:14:29  hopper
// ->Fixed a stupid error involving the rcsID string.
// ->
// ->Revision 0.19  1992/04/14  04:36:38  hopper
// ->Fixed some stuff. Added maximum size specs for
// ->FillFromFd, and PutIntoFd
// ->
// ->Revision 0.18  1992/03/20  03:58:15  hopper
// ->Removed some outdated log messages.
// ->
// ->Revision 0.17  1992/03/20  03:54:57  hopper
// ->Added class DataBlockStrChunk, and fixed some silly
// ->things. Most of the code for DataBlockStrChunk is in
// ->StrChunkio.cc
// ->
// ->Revision 0.16  1992/02/10  04:25:10  hopper
// ->Changed StrChunk::ChunkType && StrChunk::IsChunkType to work with
// ->Rev 0.13 of EHaudio++/ChunkType.h
// ->
// -> ........
// ->
// ->Revision 0.11  1991/11/19  05:45:36  hopper
// ->Had to change a delete in ~StrChunkBuffer to a free. See StrChunk.cc
// ->for reason.
// ->(StrChunk.cc 0.10 log file anyway.)
// ->

#include <assert.h>

#ifndef _STR_STR_ClassIdent_H_
#  ifndef OS2
#     include <StrMod/STR_ClassIdent.h>
#  else
#     include "str_clas.h"
#  endif
#endif

#include <LCore/Object.h>

#define _STR_StrChunk_H_

class LinearExtent;
class GroupVector;

//------------------------------class StrChunk---------------------------------

class StrChunk : public Object {
 public:
   enum KeepDir { KeepLeft, KeepRight, KeepNone };
   static const STR_ClassIdent identifier;

   StrChunk() : refcounter_(0)              { }
   virtual ~StrChunk()                      { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   void AddReference()                      { refcounter_++; }
   void DelReference()                      { if (refcounter_) refcounter_--; }
   unsigned int NumReferences() const       { return(refcounter_); }

   virtual unsigned int Length() const = 0;
   virtual unsigned int NumSubGroups() const;
   virtual unsigned int NumSubGroups(const LinearExtent &extent) const = 0;
   inline void SimpleFillGroupVec(GroupVector &vec);
   virtual void FillGroupVec(GroupVector &vec, unsigned int &start_index);
   virtual void FillGroupVec(const LinearExtent &extent,
			     GroupVector &vec, unsigned int &start_index) = 0;
   inline void DropUnused(const LinearExtent &usedextent,
			  KeepDir keepdir = KeepLeft);

 protected:
   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   void AddReferences(unsigned int num)     { refcounter_ += num; }
   inline void DelReferences(unsigned int num);

   virtual void i_DropUnused(const LinearExtent &usedextent,
			     KeepDir keepdir) = 0;

 private:
   unsigned int refcounter_;
};

//------------------------inline functions for StrChunk------------------------

inline int StrChunk::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

inline void StrChunk::SimpleFillGroupVec(GroupVector &vec)
{
   unsigned int i = 0;

   FillGroupVec(vec, i);
}

inline void StrChunk::DropUnused(const LinearExtent &usedextent,
				 KeepDir keepdir)
{
   if (NumReferences() <= 1) {
      i_DropUnused(usedextent, keepdir);
   }
}

inline void StrChunk::DelReferences(unsigned int num)
{
   if (refcounter_ > num)
      refcounter_ -= num;
   else
      refcounter_ = 0;
}

#endif
