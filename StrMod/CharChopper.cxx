/* $Header$ */

// $Log$
// Revision 1.1  1996/09/02 23:28:22  hopper
// Added CharChopper class so users would have a simple class that would
// break up and recombine streams using whatever character they chose as
// a separator.
//

#ifdef __GNUG__
#  pragma implementation "CharChopper.h"
#endif

#include <StrMod/CharChopper.h>
#include <StrMod/StrChunk.h>
#include <StrMod/StrChunkPtrT.h>
#include <StrMod/GroupChunk.h>
#include <StrMod/GroupVector.h>
#include <StrMod/GV_Iterator.h>
#include <StrMod/StrSubChunk.h>

const STR_ClassIdent CharChopper::identifier(26UL);

void CharChopper::ProcessIncoming()
{
   assert(m_incoming);
   assert(!m_outgoing_ready);

   StrChunkPtrT<GroupChunk> outchnk;

   if (m_outgoing) {
      assert(m_outgoing->AreYouA(GroupChunk::identifier));
      outchnk = static_cast<GroupChunk *>(m_outgoing.GetPtr());
   } else {
      outchnk = new GroupChunk;
      m_outgoing = outchnk;
   }

   unsigned int inlen = m_incoming->Length();

   if (inlen <= 0) {
      if (curdata) {
	 outchnk->push_back(curdata);
	 curdata.ReleasePtr();
      }
      outchnk->push_back(m_incoming);
      m_incoming.ReleasePtr();
   } else {
      GV_Size gvsize = m_incoming->NumSubGroups();
      GroupVector gvec(gvsize);

      m_incoming->SimpleFillGroupVec(gvec);

      GroupVector::Iterator i = gvec.begin();
      unsigned int count = 0;

      while (i) {
	 count++;   // To include character when we find it.
	 if (*i == chopchar_) {
	    if (curdata) {
	       outchnk->push_back(curdata);
	       curdata.ReleasePtr();
	    }
	    if (count < inlen) {
	       outchnk->push_back(new StrSubChunk(m_incoming,
						  LinearExtent(0, count)));
	       m_incoming =
		  new StrSubChunk(m_incoming, LinearExtent(count, inlen));
	    } else {
	       outchnk->push_back(m_incoming);
	       m_incoming.ReleasePtr();
	    }
	    m_outgoing_ready = true;
	    return;
	 }
	 ++i;
      }
      if (inlen < 16) {
	 unsigned int curpos;
	 unsigned char *curary;

	 if (!curdata) {
	    curdata = new DataBlockStrChunk(inlen);
	    curpos = 0;
	 } else {
	    unsigned int curlen;

	    curlen = curdata->Length();
	    curdata->Resize(curlen + inlen);
	    curpos = curlen;
	    curlen += inlen;
	 }
	 curary = curdata->GetCharP();
	 
	 i = gvec.begin();
	 while (i) {
	    curary[curpos++] = *i;
	    ++i;
	 }
      } else {
	 outchnk->push_back(m_incoming);
      }
      m_incoming.ReleasePtr();
   }
}
