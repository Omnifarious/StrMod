#include <xml/utf8/Builder.h>
#include <xml/utf8/Lexer.h>

/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */

#include <string>
#include <cstring>
#include <iostream>
#include <cstddef>
#include <stack>
#include <vector>
#include <stdexcept>

using ::strmod::xml::utf8::Lexer;
using ::strmod::xml::utf8::Builder;

namespace strmod {
namespace tests {
namespace {

using ::std::cout;

struct AttributeData
{
   ::std::string name_;
   ::std::string value_;
   ::std::string literal_;
   char delimiter_;

   AttributeData(const ::std::string &name, const ::std::string &value,
                 const ::std::string &literal, char delimiter)
        : name_(name), value_(value), literal_(literal), delimiter_(delimiter)
   {
   }
};

struct ElementData
{
   const ::std::string name_;
   size_t elbegin_;
   size_t datbegin_;
   size_t datend_;
   ::std::vector<AttributeData> attrlist_;

   ElementData(const ::std::string &name, size_t elbegin)
        : name_(name), elbegin_(elbegin), datbegin_(elbegin), datend_(elbegin)
   {
   }
};

class TestBuilder : public Builder
{
 public:
   TestBuilder(const char *buf, Lexer &lexer) : buf_(buf), lexer_(lexer) { }
   virtual ~TestBuilder() { }

   virtual void startElementTag(const Position &begin, const ::std::string &name)
   {
      elstack_.push(ElementData(name, begin.bufoffset_));
   }
   virtual void addAttribute(const Position &attrbegin, const Position &attrend,
                             const Position &valbegin, const Position &valend,
                             const ::std::string &name)
   {
      ::std::string literal(buf_ + attrbegin.bufoffset_,
                            attrend.bufoffset_ - attrbegin.bufoffset_);
      ::std::string value(buf_ + valbegin.bufoffset_,
                          valend.bufoffset_ - valbegin.bufoffset_);
      elstack_.top().attrlist_.push_back(AttributeData(name, value, literal, buf_[valend.bufoffset_]));
   }
   virtual void endElementTag(const Position &end, bool wasempty)
   {
      ElementData &eldata = elstack_.top();
      eldata.datbegin_ = end.bufoffset_;
//       ::std::cerr << "eldata.elbegin_ == " << eldata.elbegin_
//                   << " && eldata.datbegin_ == " << eldata.datbegin_ << "\n";
      ::std::string tag(buf_ + eldata.elbegin_,
                        eldata.datbegin_ - eldata.elbegin_);
//      ::std::cerr << "tag == \"" << tag << "\"\n";
      for (size_t i = 1; i < elstack_.size(); ++i) {
         cout << "   ";
      }
      cout << "<" << eldata.name_;
      if (eldata.attrlist_.size() > 0)
      {
         typedef ::std::vector<AttributeData> attrs_t;
         attrs_t &attrlist_ = eldata.attrlist_;
         for (attrs_t::iterator i = attrlist_.begin(); i != attrlist_.end(); ++i)
         {
            cout << " " << i->name_ << "="
                 << i->delimiter_ << i->value_ << i->delimiter_
                 << " :=: [" << i->literal_ << "]";
         }
      }
      if (wasempty) {
         eldata.datend_ = end.bufoffset_;
         cout << "/> :=: [" << tag << "]\n";
//          cout.flush();
         elstack_.pop();
      } else {
         cout << "> :=: [" << tag << "]\n";
//          cout.flush();
         if (eldata.name_ == "store")
         {
            lexer_.setNonWSInElements(true);
         }
      }
   }
   virtual void closeElementTag(const Position &begin,
                                const Position &end, const ::std::string &name)
   {
      ElementData &eldata = elstack_.top();
      eldata.datend_ = begin.bufoffset_;
      ::std::string data = ::std::string(buf_ + eldata.datbegin_,
                                         eldata.datend_ - eldata.datbegin_);
      ::std::string tag = ::std::string(buf_ + begin.bufoffset_,
                                        end.bufoffset_ - begin.bufoffset_);
      if (name != eldata.name_) {
         throw ::std::runtime_error("Parse error!\n");
      }
      if (name == "store")
      {
         lexer_.setNonWSInElements(false);
      }
      for (size_t i = 1; i < elstack_.size(); ++i) {
         cout << "   ";
      }
      cout << "</" << eldata.name_ << ">"
           << " :=: [" << tag << "] :=: [" << data << "]\n";
      elstack_.pop();
   }

 private:
   const char * const buf_;
   Lexer &lexer_;
   ::std::stack<ElementData> elstack_;
};

}
}
}

using strmod::tests::TestBuilder;

int main()
{
   std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
   try {
      const char * const xmlstr = "<fred> "
         "<went joe=\"'barney\" alley='\"kate'> <down> <to> "
         "<the> <street> </street> <br/> </the> "
         "<a time= 'then' ><store time='now'>The New French Bakery</store></a>"
         "</to> </down> </went> </fred>";
      const char * const xml2str =
"<fred>\n"
"   <went>\n"
"      <down>\n"
"         <to>\n"
"            <the>\n"
"               <street>\n"
"               </street>\n"
"               <br/>\n"
"            </the>\n"
"         </to>\n"
"      </down>\n"
"   </went>\n"
"</fred>\n";
      Lexer lexer;
      {
         TestBuilder ts(xmlstr, lexer);
         ::std::cout << "Parsing: [" << xmlstr << "]\n\n";
         Builder::BufHandle bh;
         bh.ulval_ = 0;
         lexer.lex(xmlstr, ::strlen(xmlstr), bh, ts);
      }
      ::std::cout << "================================\n";
      {
         TestBuilder ts(xml2str, lexer);
         ::std::cout << "Parsing: [" << xml2str << "]\n\n";
         Builder::BufHandle bh;
         bh.ulval_ = 0;
         lexer.lex(xml2str, ::strlen(xml2str), bh, ts);
      }
      return 0;
   } catch (::std::exception e) {
      ::std::cerr << "Dying of an exception exception: [" << e.what() << "]\n";
      throw;
   } catch (...) {
      ::std::cerr << "Dying of an exception!\n";
      throw;
   }
   return 0;
}

// $Log$
// Revision 1.8  2003/01/10 02:29:49  hopper
// Initial, dumb support for the multi-buffer enabled Lexer and Builder
// interfaces.
//
// Revision 1.7  2003/01/09 22:48:32  hopper
// Much farter along multiple buffer parsing.
//
// Revision 1.6  2002/12/11 21:55:41  hopper
// It parses attributes now.  There's even a decent test for it.  :-)
//
// Revision 1.5  2002/12/11 18:52:02  hopper
// More steps towards parsing attributes.
//
// Revision 1.4  2002/12/10 22:46:02  hopper
// Renamed the XMLParserStrategy to the more appropriate XMLBuilder from
// Design Patterns.
//
// Revision 1.3  2002/12/10 16:08:42  hopper
// Preliminary changes to allow elements to have #PCDATA.
//
// Revision 1.2  2002/12/10 13:21:55  hopper
// Moved log to the end, like it is for all the other files.
//
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
