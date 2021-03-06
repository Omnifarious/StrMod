/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */

#include "xml/utf8/Lexer.h"
#include "xml/utf8/Builder.h"
#include <string>
#include <stdexcept>

namespace strmod {
namespace xml {
namespace utf8 {

const ::std::string Lexer::out_of_range_message("Lexer::lex: Element, attribute or entity name too large!");
const ::std::string Lexer::bad_case_message("Lexer::lex: Programmer error!  Bad case!");

void Lexer::throw_out_of_range()
{
   throw ::std::out_of_range(out_of_range_message);
}

void Lexer::throw_bad_case()
{
   throw ::std::logic_error(bad_case_message);
}

void Lexer::call_startElementTag(const Position &first, size_t namepos,
                                        Builder &parser)
{
   parser.startElementTag(first, ::std::string(name_, namepos));
}

void Lexer::call_closeElementTag(const Position &first,
                                        const Position &last,
                                        size_t namepos, Builder &parser)
{
   parser.closeElementTag(first, last, ::std::string(name_, namepos));
}

void Lexer::call_addAttribute(const Position &attrbegin,
                                     const Position &attrend,
                                     const Position &valbegin,
                                     const Position &valend,
                                     size_t namepos, Builder &parser)
{
   parser.addAttribute(attrbegin, attrend,
                       valbegin, valend,
                       ::std::string(name_, namepos));
}

} // namespace utf8
} // namespace xml
} // namespace strmod

// $Log$
// Revision 1.6  2003/01/10 02:29:10  hopper
// Added a function to throw an exception when the main lexer routine
// encounters 'impossible' conditions.
//
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
