/* $Header$ */

// $Log$
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
//

#include <string>
#include <cstring>
#include <iostream>
#include <cstddef>
#include <stack>
#include <stdexcept>
#include "XMLUTF8Lexer.h"

namespace {

using ::std::cout;

struct ElementData
{
   const ::std::string name_;
   size_t elbegin_;
   size_t datbegin_;
   size_t datend_;

   ElementData(const ::std::string &name, size_t elbegin)
        : name_(name), elbegin_(elbegin), datbegin_(elbegin), datend_(elbegin)
   {
   }
};

class TestStrategy : public XMLParseStrategy
{
 public:
   TestStrategy(const char *buf) : buf_(buf) { }
   virtual ~TestStrategy() { }

   virtual void startElementTag(size_t begin, const ::std::string &name) {
      elstack_.push(ElementData(name, begin));
   }
   virtual void endElementTag(size_t end, bool wasempty) {
      ElementData &eldata = elstack_.top();
      eldata.datbegin_ = end;
      ::std::cerr << "eldata.elbegin_ == " << eldata.elbegin_
                  << " && eldata.datbegin_ == " << eldata.datbegin_ << "\n";
      ::std::string tag(buf_ + eldata.elbegin_,
                        eldata.datbegin_ - eldata.elbegin_);
      ::std::cerr << "tag == \"" << tag << "\"\n";
      for (size_t i = 1; i < elstack_.size(); ++i) {
         cout << "   ";
      }
      if (wasempty) {
         eldata.datend_ = end;
         cout << "<" << eldata.name_ << "/> :=: [" << tag << "]\n";
         cout.flush();
         elstack_.pop();
      } else {
         cout << "<" << eldata.name_ << "> :=: [" << tag << "]\n";
         cout.flush();
      }
   }
   virtual void closeElementTag(size_t begin, size_t end, const ::std::string &name) {
      ElementData &eldata = elstack_.top();
      eldata.datend_ = begin;
      ::std::string data = ::std::string(buf_ + eldata.datbegin_,
                                         eldata.datend_ - eldata.datbegin_);
      ::std::string tag = ::std::string(buf_ + begin, end - begin);
      if (name != eldata.name_) {
         throw ::std::runtime_error("Parse error!\n");
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
   ::std::stack<ElementData> elstack_;
};

}

int main()
{
   std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
   try {
      const char * const xmlstr = "<fred> <went> <down> <to> "
         "<the> <street> </street> <br/> </the> "
         "<a><store></store></a>"
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
      XMLUTF8Lexer lexer;
      {
         TestStrategy ts(xmlstr);
         lexer.lex(xmlstr, ::strlen(xmlstr), ts);
      }
      {
         TestStrategy ts(xml2str);
         lexer.lex(xml2str, ::strlen(xml2str), ts);
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
