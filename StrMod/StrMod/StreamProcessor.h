#ifndef _STR_StreamProcessor_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// $Log$
// Revision 1.1  1996/09/02 23:33:54  hopper
// Created abstract class (class StreamProcessor) for handling simple
// dataflows where one grabbed incoming data, did stuff to it, and sent
// the result along.
//

#include <LCore/GenTypes.h>
#include <StrMod/STR_ClassIdent.h>
#include <LCore/Protocol.h>
#include <StrMod/StrChunkPtr.h>
#include <assert.h>

#define _STR_StreamProcessor_H_

class StreamProcessor : virtual public Protocol {
 public:
   static const STR_ClassIdent identifier;

   inline StreamProcessor();
   virtual ~StreamProcessor();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline bool CanAddIncoming();
   inline void AddIncoming(const StrChunkPtr &chnk);
   inline bool CanPullOutgoing();
   inline const StrChunkPtr PullOutgoing();

 protected:
   StrChunkPtr m_outgoing;
   bool m_outgoing_ready;
   StrChunkPtr m_incoming;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual void ProcessIncoming() = 0;

 private:
   StreamProcessor(const StreamProcessor &b);
   void operator =(const StreamProcessor &b);
};  

//-----------------------------inline functions--------------------------------

inline StreamProcessor::StreamProcessor()
     : m_incoming(0), m_outgoing(0), m_outgoing_ready(false)
{
}

inline int StreamProcessor::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Protocol::AreYouA(cid));
}

inline bool StreamProcessor::CanAddIncoming()
{
   return(m_incoming == 0);
}

inline void StreamProcessor::AddIncoming(const StrChunkPtr &chnk)
{
   assert(CanAddIncoming());
   m_incoming = chnk;
   if (!CanPullOutgoing()) {
      ProcessIncoming();
   }
}

inline bool StreamProcessor::CanPullOutgoing()
{
   return(m_outgoing_ready);
}

inline const StrChunkPtr StreamProcessor::PullOutgoing()
{
   assert(CanPullOutgoing());

   StrChunkPtr tmp;

   tmp = m_outgoing;
   m_outgoing = 0;
   m_outgoing_ready = false;
   if (m_incoming) {
      ProcessIncoming();
   }
   return(tmp);
}

#endif
