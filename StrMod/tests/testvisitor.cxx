/* -*-c-file-style: "hopper";-*- */
/* $Header$ */

// For a log, see ChangeLog

#include "StrMod/GraphVizVisitor.h"
#include "StrMod/PreAllocBuffer.h"
#include "StrMod/GroupChunk.h"
#include "StrMod/StrSubChunk.h"
#include "StrMod/EOFStrChunk.h"
#include "StrMod/StrChunkPtrT.h"
#include "StrMod/ChunkIterator.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

static const char phrase0[] = "George Orwell has maggots.";
static const char phrase1[] = "George Orwell doesn't have fleas.";
static const char phrase2[] = "This is just a bunch of words so that I can piece them together to make strange sentences.";

static BufferChunk *phrases[3] = {
   new PreAllocBuffer<sizeof(phrase0) - 1>,
   new PreAllocBuffer<sizeof(phrase1) - 1>,
   new PreAllocBuffer<sizeof(phrase2) - 1>
};

void initPre()
{
   phrases[0]->resize(sizeof(phrase0) - 1);
   memcpy(phrases[0]->getVoidP(), phrase0, sizeof(phrase0) - 1);
   phrases[0]->AddReference();
//   cerr << "phrases[0]->getVoidP() == " << phrases[0]->getVoidP() << "\n";

   phrases[1]->resize(sizeof(phrase1) - 1);
   memcpy(phrases[1]->getVoidP(), phrase1, sizeof(phrase1) - 1);
   phrases[1]->AddReference();
//   cerr << "phrases[1]->getVoidP() == " << phrases[1]->getVoidP() << "\n";

   phrases[2]->resize(sizeof(phrase2) - 1);
   memcpy(phrases[2]->getVoidP(), phrase2, sizeof(phrase2) - 1);
   phrases[2]->AddReference();
//   cerr << "phrases[2]->getVoidP() == " << phrases[2]->getVoidP() << "\n";
}

static void iteratorTest(const StrChunkPtr &chnk, const StrChunkPtr &data)
{
   assert(data->AreYouA(BufferChunk::identifier));
   StrChunkPtrT<BufferChunk> bdata;
   bdata = static_cast<BufferChunk *>(data.GetPtr());
   unsigned char *chdata = bdata->getCharP();
   StrChunk::const_iterator begin = chnk->begin();
   StrChunk::const_iterator end = chnk->end();

   {
      StrChunk::const_iterator i = begin;
      int o = 0;
      for (; i != end; ++i, ++o)
      {
         assert(o < bdata->Length());
         assert(o == (i - begin));
         assert(*i == chdata[o]);
      }
      assert(o == bdata->Length());
      assert(o == chnk->Length());
   }

   {
      StrChunk::const_iterator i = begin;
      int o = 0;
      const unsigned int length = data->Length();
      const unsigned int iters = length * 10U;

      for (unsigned int p = 0; p < iters; ++p)
      {
         unsigned int r = random() % iters;
         if (r < p)
         {
//            cerr << 'D';
            --i;
            if (o > 0)
            {
               --o;
            }
         }
         else
         {
            ++i;
            if (o < length)
            {
               ++o;
            }
//            cerr << 'U';
         }
         assert((i - begin) == o);
         if (i != end)
         {
            assert(o < length);
            assert(*i == chdata[o]);
         }
         else
         {
            assert(o == length);
         }
//         cerr << o << ' ';
      }
   }
//   cerr << '\n';
}

static void nonIteratorTest(const StrChunkPtr &chnk)
{
}

int main()
{
   srandom(1);
   ofstream graphout("chunkgraph.dot");
   initPre();
   GraphVizVisitor visitor;
   visitor.visit(phrases[0], graphout);
   StrChunkPtrT<GroupChunk> group = new GroupChunk;
   group->push_back(phrases[0]);
   group->push_back(phrases[1]);
   group->push_back(phrases[2]);
   iteratorTest(group, visitor.visit(group, graphout));
   group = new GroupChunk;
   group->push_back(new StrSubChunk(phrases[0], LinearExtent(0, 18)));
   group->push_back(new StrSubChunk(phrases[1], LinearExtent(27,  5)));
   group->push_back(new StrSubChunk(phrases[2], LinearExtent(29, 43)));
   group->push_back(new StrSubChunk(phrases[0], LinearExtent(18, 8)));
   iteratorTest(group, visitor.visit(group, graphout));
   {
      GroupChunk *uber = new GroupChunk;
      {
         GroupChunk *tmp = new GroupChunk;
         tmp->push_back(phrases[0]);
         tmp->push_back(new StrSubChunk(group, LinearExtent(6, 1)));
         tmp->push_back(group);
         uber->push_back(tmp);
      }
      {
         GroupChunk *tmp = new GroupChunk;
         tmp->push_back(phrases[0]);
         tmp->push_back(new StrSubChunk(group, LinearExtent(17, 1)));
         tmp->push_back(group);
         uber->push_back(tmp);
      }
      {
         GroupChunk *tmp = new GroupChunk;
         tmp->push_back(phrases[0]);
         tmp->push_back(new StrSubChunk(group, LinearExtent(23, 1)));
         tmp->push_back(group);
         uber->push_back(tmp);
      }
      group = uber;
   }
   iteratorTest(group, visitor.visit(group, graphout));
   group = new GroupChunk;
   {
      StrChunkPtr eof = new EOFStrChunk();
      group->push_back(eof);
      group->push_back(new StrSubChunk(phrases[2], LinearExtent(0,24)));
      group->push_back(eof);
      group->push_back(new StrSubChunk(phrases[1], LinearExtent(27, 5)));
      group->push_back(eof);
      group->push_back(new StrSubChunk(phrases[0], LinearExtent(25, 1)));
      group->push_back(eof);
   }
   iteratorTest(group, visitor.visit(group, graphout));
   return(0);
}
