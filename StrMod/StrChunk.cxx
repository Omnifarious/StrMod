/* $Header$ */

// $Log$
// Revision 1.2  1996/06/29 06:44:30  hopper
// Completely re-worked for new StrChunk handling.
//
// Revision 1.1.1.1  1995/07/22 04:46:47  hopper
// Imported sources
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
#include "StrMod/LinearExtent.h"

const STR_ClassIdent StrChunk::identifier(6UL);

unsigned int StrChunk::NumSubGroups() const
{
   return(NumSubGroups(LinearExtent::full_extent));
}

void StrChunk::FillGroupVec(GroupVector &vec, unsigned int &start_index)
{
   FillGroupVec(LinearExtent::full_extent, vec, start_index);
}
