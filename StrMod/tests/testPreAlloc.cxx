#include <StrMod/PreAllocBuffer.h>
#include <StrMod/DynamicBuffer.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>

#define HAVE_MREMAP 1
#define USE_MEMCPY 1
#define REALLOC_ZERO_BYTES_FREES 1
#define DEBUG 1
#include "dlmalloc.c"

inline static void DoBufferChunk(BufferChunk &c, unsigned int size)
{
   c.Resize(size);
   if (!c.invariant())
   {
      c.printState(cerr);
      assert(false);
   }
   memset(c.getVoidP(), 'g', size);
}

static void testBufferChunk(BufferChunk &chnk)
{
   malloc_stats();
   cerr << "-----\n";
   DoBufferChunk(chnk, 100);
   DoBufferChunk(chnk, 1);
   DoBufferChunk(chnk, 11);
   DoBufferChunk(chnk, 0);
   DoBufferChunk(chnk, 3);
   DoBufferChunk(chnk, 11);
   DoBufferChunk(chnk, 0);
   DoBufferChunk(chnk, 11);
   for (int i = 0; i < 1000000; ++i)
   {
      DoBufferChunk(chnk, rand() % 32768);
   }
   DoBufferChunk(chnk, 32760);
   malloc_stats();
   cerr << "-----\n";
   DoBufferChunk(chnk, 1);
   malloc_stats();
   cerr << "-----\n";
   DoBufferChunk(chnk, 0);
   malloc_stats();
}

int main()
{
   PreAllocBuffer<5> g;
   DynamicBuffer h;

   testBufferChunk(g);
   cerr << "===============\n";
   testBufferChunk(h);
}
