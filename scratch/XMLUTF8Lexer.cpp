/* $Header$ */

#ifdef __GNUG__
#  pragma implementation "XMLUTF8Lexer.h"
#endif

#include "XMLUTF8Lexer.h"
#include <string>
#include <stdexcept>

void XMLUTF8Lexer::lex(const char *buf, unsigned int len,
                       XMLParseStrategy &parser)
{
   size_t elstart = 0;
   for (unsigned int i = 0; (i < len) && (state_ != XBad); ++i)
   {
      const XState oldstate = state_;
      const XSubState oldsubstate = substate_;
      advanceState(buf[i]);
      if (namepos_ >= sizeof(name_))
      {
         throw_out_of_range();
//         throw ::std::out_of_range(out_of_range_message);
      }
      if (state_ == XLess)
      {
         elstart = i;
      }
      if ((substate_ == XSNone) && (oldsubstate == XSInName) &&
          (oldstate == XOpenElement))
      {
         call_startElementTag(elstart, parser);
      }
      if ((state_ != oldstate) && (state_ == XStart))
      {
         if ((oldstate == XOpenElement) || (oldstate == XInOpenElement))
         {
            parser.endElementTag(i + 1, false);
         }
         else if (oldstate == XEmptyElementEnd)
         {
            parser.endElementTag(i + 1, true);
         }
         else if ((oldstate == XCloseElement) || (oldstate == XInCloseElement))
         {
            call_closeElementTag(elstart, i + 1, parser);
         }
      }
   }
}

// $Log$
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
//
