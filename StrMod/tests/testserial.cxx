#include <StrMod/OutSerializer.h>
#include <StrMod/InSerializer.h>
#include <StrMod/DBStrChunk.h>
#include <StrMod/StrChunkPtr.h>
#include <iostream.h>

int main()
{
   DataBlockStrChunk *chnk;

   {
      OutSerializer out;
      U1Byte a = 1;
      U2Byte b = 2;
      U4Byte c = 4;
      U4Byte d = 0x12345678;
      string george = "george";

      out << a << b << c << george << "orwell" << d;

      chnk = out.TakeChunk();
   }
   cout.write(chnk->GetVoidP(), chnk->Length());

   {
      StrChunkPtr chnkp = chnk;
      InSerializer in(chnk);
      U1Byte u1;
      U2Byte u2;
      U4Byte u4_1;
      U4Byte u4_2;
      string a, b;

      u1 = in.GetU1Byte();
      u2 = in.GetU2Byte();
      u4_1 = in.GetU4Byte();
      a = in.GetString();
      b = in.GetString();
      u4_2 = in.GetU4Byte();

      if (in.HadError()) {
	 cerr << "Had Error!\n";
      } else {
	 cerr << "Went OK!\n";
      }
      cerr << "in.BytesLeft() == " << in.BytesLeft() << "\n";
      cerr << "u1 == " << (unsigned int)(u1) << "\n";
      cerr << "u2 == " << u2 << "\n";
      cerr << "u4_1 == " << u4_1 << "\n";
      cerr << "a == \"" << a << "\"\n";
      cerr << "b == \"" << b << "\"\n";
      cerr << "u4_2 == " << u4_2 << "\n";
   }
   {
      char buf[] = { 0x01, 0x02, 0x00, 0x04, 0x03, 0x02, 0x01,
		     0x00, 0x04, 'h', 'a', 't', '\0', 0x01, 0x00, 0x02, 0x03 };
      InSerializer in(buf, sizeof(buf));
      U1Byte u1;
      U2Byte u2;
      U4Byte u4_1;
      U4Byte u4_2;
      string a;

      u1 = in.GetU1Byte();
      u2 = in.GetU2Byte();
      u4_1 = in.GetU4Byte();
      a = in.GetString();
      u4_2 = in.GetU4Byte();

      if (in.HadError()) {
	 cerr << "Had Error!\n";
      } else {
	 cerr << "Went OK!\n";
      }
      cerr << "in.BytesLeft() == " << in.BytesLeft() << "\n";
      cerr << "u1 == " << (unsigned int)(u1) << "\n";
      cerr << "u2 == " << u2 << "\n";
      cerr << "u4_1 == " << u4_1 << "\n";
      cerr << "a == \"" << a << "\"\n";
      cerr << "u4_2 == " << u4_2 << "\n";
   }
}

