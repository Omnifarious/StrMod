/* -*-c-file-style: "hopper";-*- */
/* $Header$ */

// For a log, see ChangeLog

#include "StrMod/GraphVizVisitor.h"
#include "StrMod/PreAllocBuffer.h"
#include "StrMod/GroupChunk.h"
#include "StrMod/StrSubChunk.h"
#include "StrMod/EOFStrChunk.h"
#include "StrMod/StrChunkPtrT.h"
#include <iostream>

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

int main()
{
   initPre();
   GraphVizVisitor visitor;
   visitor.visit(phrases[0], cout);
   StrChunkPtrT<GroupChunk> group = new GroupChunk;
   group->push_back(phrases[0]);
   group->push_back(phrases[1]);
   group->push_back(phrases[2]);
   visitor.visit(group, cout);
   group = new GroupChunk;
   group->push_back(new StrSubChunk(phrases[0], LinearExtent(0, 18)));
   group->push_back(new StrSubChunk(phrases[1], LinearExtent(27,  5)));
   group->push_back(new StrSubChunk(phrases[2], LinearExtent(29, 43)));
   group->push_back(new StrSubChunk(phrases[0], LinearExtent(18, 8)));
   visitor.visit(group, cout);
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
   visitor.visit(group, cout);
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
   visitor.visit(group, cout);
   return(0);
}
