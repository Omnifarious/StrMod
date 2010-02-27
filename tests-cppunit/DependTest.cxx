/*
 * Copyright 2002-2010 Eric M. Hopper <hopper@omnifarious.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */

// For a log, see ChangeLog

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

namespace strmod {
namespace tests {

using ::system;
using ::unlink;
using ::std::string;
using ::std::ofstream;
using ::std::ostream;

class DependTest : public ::CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE(DependTest);
   CPPUNIT_TEST(testContents_lcore);
   CPPUNIT_TEST(testContents_ehnet);
   CPPUNIT_TEST(testContents_unievent);
   CPPUNIT_TEST(testContents_strmod);
   CPPUNIT_TEST(testContents_xml);
   CPPUNIT_TEST(testDepends_lcore);
   CPPUNIT_TEST(testDepends_ehnet);
   CPPUNIT_TEST(testDepends_unievent);
   CPPUNIT_TEST(testDepends_strmod);
   CPPUNIT_TEST(testDepends_xml);
   CPPUNIT_TEST_SUITE_END();
 private:
   static const string S_objectdir;
   static const string S_ls_lcore[21];
   static const string S_ls_ehnet[6];
   static const string S_ls_unievent[12];
   static const string S_ls_strmod[42];
   static const string S_ls_xml[3];

 public:
   void setUp();
   void tearDown();

   void testContents_lcore();
   void testContents_ehnet();
   void testContents_unievent();
   void testContents_strmod();
   void testContents_xml();

   void testDepends_lcore();
   void testDepends_ehnet();
   void testDepends_unievent();
   void testDepends_strmod();
   void testDepends_xml();

 protected:
   void checkDepends(const string &file, const string &ok_namespaces_regex);

 private:
   void dumpStrings(ostream &os, const string strings[], int size);
};

CPPUNIT_TEST_SUITE_REGISTRATION(DependTest);

void DependTest::setUp()
{
   {
      ofstream out("ls_lcore.txt");
      dumpStrings(out, S_ls_lcore, sizeof(S_ls_lcore) / sizeof(string));
   }
   {
      ofstream out("ls_ehnet.txt");
      dumpStrings(out, S_ls_ehnet, sizeof(S_ls_ehnet) / sizeof(string));
   }
   {
      ofstream out("ls_unievent.txt");
      dumpStrings(out, S_ls_unievent, sizeof(S_ls_unievent) / sizeof(string));
   }
   {
      ofstream out("ls_strmod.txt");
      dumpStrings(out, S_ls_strmod, sizeof(S_ls_strmod) / sizeof(string));
   }
   {
      ofstream out("ls_xml.txt");
      dumpStrings(out, S_ls_xml, sizeof(S_ls_xml) / sizeof(string));
   }
}

void DependTest::tearDown()
{
   unlink("ls_lcore.txt");
   unlink("ls_ehnet.txt");
   unlink("ls_unievent.txt");
   unlink("ls_strmod.txt");
   unlink("ls_xml.txt");
}

void DependTest::testContents_lcore()
{
   string cmdline("export LC_COLLATE=C; (cd '" + S_objectdir + "'; /bin/ls LCore_*) | /usr/bin/diff - ls_lcore.txt >/dev/null 2>/dev/null");
   int sysret = system(cmdline.c_str());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("LCore .o files don't match list in S_ls_lcore.", 0, sysret);
}

void DependTest::testContents_ehnet()
{
   string cmdline("export LC_COLLATE=C; (cd '" + S_objectdir + "'; /bin/ls 'EHnet++_'*) | /usr/bin/diff - ls_ehnet.txt >/dev/null 2>/dev/null");
   int sysret = system(cmdline.c_str());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("EHNet++ .o files don't match list in S_ls_ehnet", 0, sysret);
}

void DependTest::testContents_unievent()
{
   string cmdline("export LC_COLLATE=C; (cd '" + S_objectdir + "'; /bin/ls UniEvent_*) | /usr/bin/diff - ls_unievent.txt >/dev/null 2>/dev/null");
   int sysret = system(cmdline.c_str());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("UniEvent .o files don't match list in S_ls_unievent.", 0, sysret);
}

void DependTest::testContents_strmod()
{
   string cmdline("export LC_COLLATE=C; (cd '" + S_objectdir + "'; /bin/ls StrMod_*) | /usr/bin/diff - ls_strmod.txt >/dev/null 2>/dev/null");
   int sysret = system(cmdline.c_str());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("StrMod .o files don't match list in S_ls_strmod.", 0, sysret);
}

void DependTest::testContents_xml()
{
   string cmdline("export LC_COLLATE=C; (cd '" + S_objectdir + "'; /bin/ls xml_*) | /usr/bin/diff - ls_xml.txt >/dev/null 2>/dev/null");
   int sysret = system(cmdline.c_str());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("xml .o files don't match list in S_ls_xml.", 0, sysret);
}

void DependTest::testDepends_lcore()
{
   const string ok_names = "(std|strmod::lcore)::";
   for (unsigned int i = 0; i < (sizeof(S_ls_lcore) / sizeof(string)); ++i)
   {
      checkDepends(S_ls_lcore[i], ok_names);
   }
}

void DependTest::testDepends_ehnet()
{
   const string ok_names = "(std|strmod::(lcore|ehnet))::";
   for (unsigned int i = 0; i < (sizeof(S_ls_ehnet) / sizeof(string)); ++i)
   {
      checkDepends(S_ls_ehnet[i], ok_names);
   }
}

void DependTest::testDepends_unievent()
{
   const string ok_names = "(std|strmod::(lcore|unievent))::";
   for (unsigned int i = 0; i < (sizeof(S_ls_unievent) / sizeof(string)); ++i)
   {
      checkDepends(S_ls_unievent[i], ok_names);
   }
}

void DependTest::testDepends_strmod()
{
   const string ok_names = "(std|strmod::(lcore|unievent|strmod|ehnet))::";
   for (unsigned int i = 0; i < (sizeof(S_ls_strmod) / sizeof(string)); ++i)
   {
      checkDepends(S_ls_strmod[i], ok_names);
   }
}

void DependTest::testDepends_xml()
{
   const string ok_names = "(std|strmod::xml)::";
   for (unsigned int i = 0; i < (sizeof(S_ls_xml) / sizeof(string)); ++i)
   {
      checkDepends(S_ls_xml[i], ok_names);
   }
}

void DependTest::checkDepends(const string &file,
                              const string &ok_namespaces_regex)
{
   string fullfile = S_objectdir;
   fullfile += "/";
   fullfile += file;
   string cmd = "nm --demangle '";
   cmd += fullfile;
   cmd += "' | egrep -v '\\b(";
   cmd += ok_namespaces_regex;
   cmd += ")' | egrep '\\b(([A-Za-z])|(_[A-Za-z0-9]))[A-Za-z0-9_]*::'"
      " >/dev/null 2>/dev/null";

   system(("echo \"" + cmd + "\" >>/tmp/log").c_str());
   int sysret = system(cmd.c_str());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Bad dependencies in " + fullfile, 256, sysret);
}

void DependTest::dumpStrings(ostream &os, const string strings[], int size)
{
   for (int i = 0; i < size; ++i)
   {
      os << strings[i] << '\n';
   }
}

const string DependTest::S_objectdir = "../generated/obj";

const string DependTest::S_ls_lcore[21] = {
   "LCore_classimp.o",
   "LCore_clsprgid.o",
   "LCore_cltpsstr.o",
   "LCore_debugabl.o",
   "LCore_defeh_cl.o",
   "LCore_defproto.o",
   "LCore_defrefco.o",
   "LCore_defspscl.o",
   "LCore_enum_set.o",
   "LCore_gentyimp.o",
   "LCore_hopclimp.o",
   "LCore_lcerror.o",
   "LCore_objctstd.o",
   "LCore_objctstr.o",
   "LCore_objecimp.o",
   "LCore_progrimp.o",
   "LCore_protoimp.o",
   "LCore_refcoimp.o",
   "LCore_rfcntptr.o",
   "LCore_rfcntptt.o",
   "LCore_simple_bitset.o"
};

const string DependTest::S_ls_ehnet[6] = {
   "EHnet++_InetAddrDef.o",
   "EHnet++_InetAddrNetDB.o",
   "EHnet++_InetAddress.o",
   "EHnet++_NET_ClassIdent.o",
   "EHnet++_SocketAddress.o",
   "EHnet++_SocketAddressDef.o"
};

const string DependTest::S_ls_unievent[12] = {
   "UniEvent_Dispatcher.o",
   "UniEvent_Event.o",
   "UniEvent_EventPtr.o",
   "UniEvent_RegistryDispatcherGlue.o",
   "UniEvent_SimpleDispatcher.o",
   "UniEvent_SimpleDispatcherImp.o",
   "UniEvent_Timer.o",
   "UniEvent_TimerEventTracker.o",
   "UniEvent_UNEVT_ClassIdent.o",
   "UniEvent_UNIXError.o",
   "UniEvent_UnixEventPoll.o",
   "UniEvent_UnixEventRegistry.o"
};

const string DependTest::S_ls_strmod[42] = {
   "StrMod_ApplyVisitor.o",
   "StrMod_BufferChunk.o",
   "StrMod_BufferChunkFactory.o",
   "StrMod_CharChopper.o",
   "StrMod_ChunkIterator.o",
   "StrMod_ChunkVisitor.o",
   "StrMod_DynamicBuffer.o",
   "StrMod_EOFStrChunk.o",
   "StrMod_EchoModule.o",
   "StrMod_FDUtil.o",
   "StrMod_GraphVizVisitor.o",
   "StrMod_GroupChunk.o",
   "StrMod_InSerializer.o",
   "StrMod_InfiniteModule.o",
   "StrMod_LinearExtent.o",
   "StrMod_LinearExtentStrmOp.o",
   "StrMod_LocalCopy.o",
   "StrMod_NewlineChopper.o",
   "StrMod_OutSerializer.o",
   "StrMod_PassThrough.o",
   "StrMod_PreAllocBuffer.o",
   "StrMod_ProcessorModule.o",
   "StrMod_RouterModule.o",
   "StrMod_STR_ClassIdent.o",
   "StrMod_SimpleMulti.o",
   "StrMod_SimpleTelnetClient.o",
   "StrMod_SockListenModule.o",
   "StrMod_SocketModule.o",
   "StrMod_StaticBuffer.o",
   "StrMod_StrChunk.o",
   "StrMod_StrChunkPtr.o",
   "StrMod_StrChunkPtrT.o",
   "StrMod_StrSubChunk.o",
   "StrMod_StreamFDModule.o",
   "StrMod_StreamModule.o",
   "StrMod_StreamProcessor.o",
   "StrMod_StreamSplitter.o",
   "StrMod_TelnetChars.o",
   "StrMod_TelnetChunkBuilder.o",
   "StrMod_TelnetChunker.o",
   "StrMod_TelnetParser.o",
   "StrMod_UseTrackingVisitor.o"
};

const string DependTest::S_ls_xml[3] = {
   "xml_utf8_Builder.o",
   "xml_utf8_Lexer.o",
   "xml_utf8_LexerSane.o"
};

} // namespace strmod::tests
} // namespace strmod
