/* $Header$ */

// $Log$
// Revision 1.1  1995/07/22 04:46:47  hopper
// Initial revision
//
// -> Revision 0.19  1995/04/14  17:05:27  hopper
// -> Combined version 0.18 and 0.18.0.5
// ->
// -> Revision 0.18.0.5  1995/04/14  16:45:58  hopper
// -> Changed things for integration into the rest of my libraries.
// ->
// -> Revision 0.18.0.4  1995/04/05  03:11:27  hopper
// -> Changed things for integration into the rest of my libraries.
// ->
// -> Revision 0.18.0.3  1995/01/23  15:55:40  hopper
// -> Added some checking so it would never try to malloc a 0 size block.
// ->
// -> Revision 0.18.0.2  1994/07/18  03:44:25  hopper
// -> Added #pragma implementation thing so this would work better with gcc 2.6.0
// ->
// -> Revision 0.18.0.1  1994/05/16  05:48:07  hopper
// -> No changes, but I needed a head for the inevitable WinterFire-OS/2
// -> branch.
// ->
// -> Revision 0.18  1994/05/07  03:24:53  hopper
// -> Changed header files stuff around to be aprroximately right with
// -> new libraries, and new names & stuff.
// ->
// -> Revision 0.17  1992/05/31  21:52:16  hopper
// -> Made a couple of modifications to get it to work on an SGI
// ->
// -> Revision 0.16  1991/11/24  22:57:30  hopper
// -> Fixed some silly syntax errors.
// ->
// -> Revision 0.15  1991/11/21  19:47:35  acm
// -> Fixed some stuff involving lines that are too long.
// ->
// -> Revision 0.14  1991/11/21  05:38:14  hopper
// -> Added some important (yeah, right) comments, and the RCS id string.
// ->
// -> Revision 0.13  1991/11/21  04:03:46  hopper
// -> Fixed some errors mostly. Added
// -> StrChunkBuffer::StrChunkBuffer(const StrChunkBuffer &)
// -> which I had previously forgotten.
// ->
// -> Revision 0.12  1991/11/20  01:19:32  hopper
// -> Changed StrChunkBuffer::operator =(const StrChunkBuffer &b) to use Resize
// -> to reallocate storage. I also added a check in there for cases like
// -> a = a;
// ->
// -> Revision 0.11  1991/11/19  05:42:53  hopper
// -> Stupid me started using delete instead of free. *bonk*
// ->
// -> Revision 0.10  1991/11/19  05:37:40  hopper
// -> Genesis!
// -> I ended up having to use malloc and realloc for memory management.
// -> *sigh* C++ doesn't have a re-new, and realloc can make use of private
// -> information about memory to heavily optimize itself sometimes.
// -> These modules, more than any others, need to be FAST.
// ->

#ifdef __GNUG__
#  pragma implementation "StrChunk.h"
#endif

#include "StrMod/StrChunk.h"
#include <iostream.h>

/*****************************************************************************
 * I don't want to use malloc, free, and realloc, but C++ lacks a            *
 * re-allocation operator, and C can sometimes be VERY efficient with        *
 * realloc. The best I could hope to do with C++ 'new' was match the worst   *
 * case for realloc. Someday I'll add some really spectacular memory         *
 * handling stuff into here, but not now, I need to get this finished. :-)   *
 *****************************************************************************/

static char *StrChunk_CC_rcsID = "$Id$";

const STR_ClassIdent StrChunkBuffer::identifier(5UL);

void StrChunkBuffer::Resize(unsigned int size)
{
   if (size == length)
      return;
   if (size == 0) {
      free(data);
      data = 0;
      length = 0;
   } else {
      if (data == 0)
	 data = malloc(size);
      else
	 data = realloc(data, size);
      if (data == 0) {
	 cerr << "Help! Out of memory in 'void StrChunkBuffer::";
	 cerr << "Resize(unsigned int size)'\n";
	 length = 0;
      } else
	 length = size;
   }
}

const StrChunkBuffer &StrChunkBuffer::operator =(const StrChunkBuffer &b)
{
   if (this != &b) {
      Resize(b.length);
      if (length != 0)
	 memcpy(data, b.data, length);
   }
   return(*this);
}

StrChunkBuffer::StrChunkBuffer(const void *dta, unsigned int size)
  : ReferenceCounting(0)
{
   length = size;
   if (length <= 0)
      data = 0;
   else {
      data = malloc(length);
      if (data == 0) {
	 cerr << "Help! Out of memory in:\n";
	 cerr << " 'StrChunkBuffer::StrChunkBuffer";
	 cerr << "(const void *dta, unsigned int size)'\n";
	 length = 0;
      } else
	 memcpy(data, dta, length);
   }
}

StrChunkBuffer::StrChunkBuffer(const StrChunkBuffer &b)
  : ReferenceCounting(0)
{
   length = b.length;
   if (length <= 0)
      data = 0;
   else {
      data = malloc(length);
      if (data == 0) {
	 cerr << "Help! Out of memory in: 'StrChunkBuffer::";
	 cerr << "StrChunkBuffer(const StrChunkBuffer &b)'\n";
	 length = 0;
      } else
	 memcpy(data, b.data, length);
   }
}

StrChunkBuffer::StrChunkBuffer(unsigned int size)
  : ReferenceCounting(0)
{
   length = size;
   if (length <= 0)
      data = 0;
   else {
      data = malloc(length);
      if (data == 0) {
	 cerr << "Help! Out of memory in 'StrChunkBuffer::";
	 cerr << "StrChunkBuffer(unsigned int size)'\n";
	 length = 0;
      }
   }
}
