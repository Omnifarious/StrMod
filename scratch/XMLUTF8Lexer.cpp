/* $Header$ */

#ifdef __GNUG__
#  pragma implementation "XMLUTF8Lexer.h"
#endif

#include "XMLUTF8Lexer.h"
#include <string>
#include <stdexcept>

bool XMLUTF8Lexer::lex(const char *buf, unsigned int len,
                       BufHandle &lastbuf, XMLBuilder &parser)
{
   LocalState stackstate(localstate_);
   for (unsigned int i = 0; (i < len) && (stackstate.state_ != XBad); ++i)
   {
      advanceState(buf[i], i, lastbuf, stackstate, parser);
   }
   localstate_ = stackstate;
   if (stackstate.used_elbegin_)
   {
      lastbuf = stackstate.elbegin_.bufhdl_;
      return true;
   }
   else if (stackstate.used_attr_)
   {
      lastbuf = stackstate.attrbegin_.bufhdl_;
      return true;
   }
   return false;
}

// $Log$
// Revision 1.5  2003/01/09 22:48:32  hopper
// Much farter along multiple buffer parsing.
//
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
