/* $Header$ */

#include "XMLUTF8Lexer.h"
#include <string>
#include <stdexcept>

const ::std::string XMLUTF8Lexer::out_of_range_message("XMLUTF8Lexer::lex: Element, attribute or entity name too large!");

void XMLUTF8Lexer::throw_out_of_range()
{
   throw ::std::out_of_range(out_of_range_message);
}

void XMLUTF8Lexer::call_startElementTag(size_t first, XMLBuilder &parser)
{
   parser.startElementTag(first, ::std::string(name_, namepos_));
}

void XMLUTF8Lexer::call_closeElementTag(size_t first, size_t last,
                                        XMLBuilder &parser)
{
   parser.closeElementTag(first, last, ::std::string(name_, namepos_));
}

void XMLUTF8Lexer::call_addAttribute(size_t attrbegin, size_t attrend,
                                     size_t valbegin, size_t valend,
                                     XMLBuilder &parser)
{
   parser.addAttribute(attrbegin, attrend,
                       valbegin, valend,
                       ::std::string(name_, namepos_));
}


// $Log$
// Revision 1.3  2002/12/11 18:52:02  hopper
// More steps towards parsing attributes.
//
// Revision 1.2  2002/12/10 22:47:34  hopper
// Renamed the XMLParserStrategy to the more appropriate XMLBuilder from
// Design Patterns.
//
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
//
