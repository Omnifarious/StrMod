#include <xml/utf8/Builder.h>
#include <xml/utf8/Lexer.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */

#include <string>
#include <cstring>
#include <cstddef>
#include <stack>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>

using ::strmod::xml::utf8::Lexer;
using ::strmod::xml::utf8::Builder;
typedef Builder::Position Position;
typedef Builder::BufHandle BufHandle;
using ::std::string;

namespace strmod {
namespace tests {

namespace {

struct AttributeData
{
   string name_;
   string value_;
   string literal_;
   char delimiter_;

   AttributeData(const string &name, const string &value,
                 const string &literal, char delimiter)
        : name_(name), value_(value), literal_(literal), delimiter_(delimiter)
   {
   }
};

typedef ::std::vector<AttributeData> attrlist_t;

struct ElementData
{
   string name_;
   Position elbegin_;
   Position datbegin_;
   Position datend_;
   ::std::vector<AttributeData> attrlist_;

   ElementData(const string &name, Position elbegin)
        : name_(name), elbegin_(elbegin), datbegin_(elbegin), datend_(elbegin)
   {
   }
};

class TestBuilder : public Builder
{
 public:
   TestBuilder(const char *buf, size_t blen, size_t chnklen,
               const string &goodparse, Lexer &lexer)
        : buf_(buf), blen_(blen), blen2_(blen / 2), chnklen_(chnklen),
          lexer_(lexer), goodparse_(goodparse)
   {
      CPPUNIT_ASSERT(blen % 2 == 0);
      {
         unsigned int a = blen2_, b = chnklen;
         unsigned int rem = a % b;
         while (rem != 0)
         {
            a = b;
            b = rem;
            rem = a % b;
         }
         CPPUNIT_ASSERT((b == 1) || (blen2_ == chnklen));
      }
   }

   virtual void startElementTag(const Position &selbegin, const string &name);
   virtual void addAttribute(const Position &attrbegin, const Position &attrend,
                             const Position &valbegin, const Position &valend,
                             const string &name);
   virtual void endElementTag(const Position &selend, bool wasempty);
   virtual void closeElementTag(const Position &celbegin,
                                const Position &celend,
                                const string &name);
 private:
   typedef ::std::stack<ElementData> elstack_t;

   const char * const buf_;
   const size_t blen_;
   const size_t blen2_;
   size_t chnklen_;
   elstack_t elements_;
   ::std::ostringstream result_;
   Lexer &lexer_;
   const string goodparse_;

   const string buildString(const Position &begin, const Position &end);
   void elementFinished(const Position &celend);
};

void TestBuilder::startElementTag(const Position &selbegin, const string &name)
{
   elements_.push(ElementData(name, selbegin));
}

void TestBuilder::addAttribute(const Position &attrbegin,
                               const Position &attrend,
                               const Position &valbegin,
                               const Position &valend, const string &name)
{
   CPPUNIT_ASSERT(elements_.size() > 0);
//   ::std::cerr << "Adding attribute: " << name << " to element: " << elements_.top().name_ << "\n";
   string literal(buildString(attrbegin, attrend));
   elements_.top().attrlist_.push_back(
      AttributeData(name, buildString(valbegin, valend), literal,
                    literal.at(literal.size() - 1)));
}

void TestBuilder::endElementTag(const Position &selend, bool wasempty)
{
   CPPUNIT_ASSERT(elements_.size() > 0);
   ElementData &el = elements_.top();
   el.datbegin_ = selend;
   if (wasempty)
   {
      el.datend_ = selend;
//      ::std::cerr << "Ended " << el.name_ << "\n";
      elementFinished(selend);
   }
   else
   {
      if (el.name_ == "store")
      {
//         ::std::cerr << "Non whitespace OK!\n";
         lexer_.setNonWSInElements(true);
      }
//      ::std::cerr << "Ended " << el.name_ << "\n";
   }
}

void TestBuilder::closeElementTag(const Position &celbegin,
                                  const Position &celend,
                                  const string &name)
{
   CPPUNIT_ASSERT(elements_.size() > 0);
   ElementData &el = elements_.top();
   CPPUNIT_ASSERT(el.name_ == name);
   el.datend_ = celbegin;
   if (el.name_ == "store")
   {
//      ::std::cerr << "Non whitespace not OK!\n";
      lexer_.setNonWSInElements(false);
   }
   elementFinished(celend);
}

const string TestBuilder::buildString(const Position &begin,
                                      const Position &end)
{
   const unsigned int begpos =
      (begin.bufhdl_.ulval_ * chnklen_ + begin.bufoffset_) % blen2_;
   unsigned int endpos =
      (end.bufhdl_.ulval_ * chnklen_ + end.bufoffset_) % blen2_;
   if (begpos > endpos)
   {
      endpos += blen2_;
   }
   CPPUNIT_ASSERT(begpos <= endpos);
   return string(buf_ + begpos, endpos - begpos);
}

void TestBuilder::elementFinished(const Position &celend)
{
   CPPUNIT_ASSERT(elements_.size() > 0);
   {
      ElementData &el = elements_.top();
      const unsigned int level = elements_.size() - 1;
      result_ << level << " !--> "
              << el.name_ << " | "
              << buildString(el.elbegin_, el.datbegin_) << "\n";
      for (attrlist_t::iterator i = el.attrlist_.begin();
           i != el.attrlist_.end();
           ++i)
      {
         AttributeData &attr = *i;
         result_ << level << " A--> "
                 << attr.name_ << " | "
                 << attr.delimiter_ << attr.value_ << attr.delimiter_ << " | "
                 << attr.literal_ << "\n";
      }
      result_ << level << " V--> "
              << buildString(el.datbegin_, el.datend_) << "\n";
      result_ << level << " C--> "
              << buildString(el.datend_, celend) << "\n\n";
   }
   elements_.pop();
   if (elements_.size() == 0)
   {
      CPPUNIT_ASSERT(result_.str() == goodparse_);
//       ::std::ofstream of("fred.out");
//       of << result_.str();
      string empty;
      result_.str(empty);
   }
   else
   {
//      ::std::ofstream of("fred.out");
//       ::std::cerr << result_.str();
   }
}

}

class XMLUTF8Test : public ::CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE(XMLUTF8Test);
   CPPUNIT_TEST(testSingleScan);
   CPPUNIT_TEST(testManyChunkSizes);
   CPPUNIT_TEST_SUITE_END();
 private:
   static const char S_xmlstr[383];
   static const char S_goodparse[1226];

 public:
   void testSingleScan();
   void testManyChunkSizes();
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMLUTF8Test);

void XMLUTF8Test::testSingleScan()
{
   Lexer lexer;
   TestBuilder builder(S_xmlstr, sizeof(S_xmlstr) - 1, sizeof(S_xmlstr) / 2, S_goodparse, lexer);
   Builder::BufHandle buf;
   buf.ulval_ = 0;
   lexer.lex(S_xmlstr, sizeof(S_xmlstr) / 2, buf, builder);
   buf.ulval_ = 1;
   lexer.lex(S_xmlstr, sizeof(S_xmlstr), buf, builder);
}

void XMLUTF8Test::testManyChunkSizes()
{
   for (unsigned int chnksize = 3;
        chnksize < ((sizeof(S_xmlstr) / 2) - 1);
        ++chnksize)
   {
      Lexer lexer;
      TestBuilder builder(S_xmlstr, sizeof(S_xmlstr) - 1, chnksize, S_goodparse, lexer);
      unsigned int bufnum = 0;
      unsigned int bufpos = 0;
      do {
         Builder::BufHandle bhdl;
         bhdl.ulval_ = bufnum;
         lexer.lex(S_xmlstr + bufpos, chnksize, bhdl, builder);
         bufpos = (bufpos + chnksize) % (sizeof(S_xmlstr) / 2);
         ++bufnum;
      } while (bufpos != 0);
   }
}

const char XMLUTF8Test::S_xmlstr[383] = "<fred> <went joe=\"'barney\"\nalley='\"kate'> <down> <to> <the> <street> </street> <br/> </the> <a time= 'then' ><store time='now'>The New French Bakery</store\t></a>\r</to> </down> </went>\t</fred><fred> <went joe=\"'barney\"\nalley='\"kate'> <down> <to> <the> <street> </street> <br/> </the> <a time= 'then' ><store time='now'>The New French Bakery</store\t></a>\r</to> </down> </went>\t</fred>";

const char XMLUTF8Test::S_goodparse[1226] = "5 !--> street | <street>\n5 V-->  \n5 C--> </street>\n\n5 !--> br | <br/>\n5 V--> \n5 C--> \n\n4 !--> the | <the>\n4 V-->  <street> </street> <br/> \n4 C--> </the>\n\n5 !--> store | <store time='now'>\n5 A--> time | 'now' | time='now'\n5 V--> The New French Bakery\n5 C--> </store\t>\n\n4 !--> a | <a time= 'then' >\n4 A--> time | 'then' | time= 'then'\n4 V--> <store time='now'>The New French Bakery</store\t>\n4 C--> </a>\n\n3 !--> to | <to>\n3 V-->  <the> <street> </street> <br/> </the> <a time= 'then' ><store time='now'>The New French Bakery</store\t></a>\r\n3 C--> </to>\n\n2 !--> down | <down>\n2 V-->  <to> <the> <street> </street> <br/> </the> <a time= 'then' ><store time='now'>The New French Bakery</store\t></a>\r</to> \n2 C--> </down>\n\n1 !--> went | <went joe=\"'barney\"\nalley='\"kate'>\n1 A--> joe | \"'barney\" | joe=\"'barney\"\n1 A--> alley | '\"kate' | alley='\"kate'\n1 V-->  <down> <to> <the> <street> </street> <br/> </the> <a time= 'then' ><store time='now'>The New French Bakery</store\t></a>\r</to> </down> \n1 C--> </went>\n\n0 !--> fred | <fred>\n0 V-->  <went joe=\"'barney\"\nalley='\"kate'> <down> <to> <the> <street> </street> <br/> </the> <a time= 'then' ><store time='now'>The New French Bakery</store\t></a>\r</to> </down> </went>\t\n0 C--> </fred>\n\n";

}
}
