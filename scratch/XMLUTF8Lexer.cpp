/* $Header$ */

#ifdef __GNUG__
#  pragma implementation "XMLUTF8Lexer.h"
#endif

#include "XMLUTF8Lexer.h"
#include <string>
#include <stdexcept>

void XMLUTF8Lexer::lex(const char *buf, unsigned int len, XMLBuilder &parser)
{
   LocalState stackstate(localstate_);
   for (unsigned int i = 0; (i < len) && (stackstate.state_ != XBad); ++i)
   {
      advanceState(buf[i], i, stackstate, parser);
   }
   localstate_ = stackstate;
}

// $Log$
// Revision 1.4  2002/12/11 21:55:41  hopper
// It parses attributes now.  There's even a decent test for it.  :-)
//
// Revision 1.3  2002/12/10 22:46:02  hopper
// Renamed the XMLParserStrategy to the more appropriate XMLBuilder from
// Design Patterns.
//
// Revision 1.2  2002/12/10 13:23:38  hopper
// No commented out lines, use version control instead, that's what it's
// for.
//
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
//
