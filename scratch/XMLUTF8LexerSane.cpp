/* $Header$ */

#include "XMLUTF8Lexer.h"
#include <string>
#include <stdexcept>

const ::std::string XMLUTF8Lexer::out_of_range_message("XMLUTF8Lexer::lex: Element, attribute or entity name too large!");

void XMLUTF8Lexer::throw_out_of_range()
{
   throw ::std::out_of_range(out_of_range_message);
}

void XMLUTF8Lexer::call_startElementTag(const Position &first, size_t namepos,
                                        XMLBuilder &parser)
{
   parser.startElementTag(first, ::std::string(name_, namepos));
}

void XMLUTF8Lexer::call_closeElementTag(const Position &first,
                                        const Position &last,
                                        size_t namepos, XMLBuilder &parser)
{
   parser.closeElementTag(first, last, ::std::string(name_, namepos));
}

void XMLUTF8Lexer::call_addAttribute(const Position &attrbegin,
                                     const Position &attrend,
                                     const Position &valbegin,
                                     const Position &valend,
                                     size_t namepos, XMLBuilder &parser)
{
   parser.addAttribute(attrbegin, attrend,
                       valbegin, valend,
                       ::std::string(name_, namepos));
}


// $Log$
// Revision 1.5  2003/01/09 22:48:32  hopper
// Much farter along multiple buffer parsing.
//
// Revision 1.4  2002/12/11 21:55:41  hopper
// It parses attributes now.  There's even a decent test for it.  :-)
//
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
