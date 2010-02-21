/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */

#ifdef __GNUG__
#  pragma implementation "Lexer.h"
#endif

#include "xml/utf8/Lexer.h"
#include <string>
#include <stdexcept>

namespace strmod {
namespace xml {
namespace utf8 {

inline bool Lexer::iswhite(const char c)
{
   return
      (c == '\x20') || // Space in UTF8, ' ' depends on encoding used by C compiler.
      (c == '\x9') ||  // Tab
      (c == '\xA') || // newline
      (c == '\xD');  // carraige return
}

inline bool Lexer::isnamestart(const char c)
{
   return
      (c == colon) || // :
      ((c >= '\x41') && (c <= '\x5A')) || // A-Z
      (c == '\x5F') || // _
      ((c >= '\x61') && (c <= '\x7A')) || // a-z
      ((c >= '\xC0') && (c <= '\xD6')) || // À-Ö
      ((c >= '\xD8') && (c <= '\xF6')) || // Ø-ö
      ((c >= '\xF8') && (c <= '\xFF')); // ø-ÿ
}

inline bool Lexer::isnamebody(const char c)
{
   return
      isnamestart(c) ||
      ((c >= '\x30') && (c <= '\x39')) || // 0-9
      (c == dot) || (c == dash) ||
      (c == '\xB7');  // Extender, whatever that is.
}

inline bool Lexer::isdigit(const char c)
{
   return (c >= '\x30') && (c <= '\x39');  // 0-9
}

inline bool Lexer::isxdigit(const char c)
{
   return
      isdigit(c) ||
      ((c >= '\x41') && (c <= '\x46')) || // A-F
      ((c >= '\x61') && (c <= '\x66'));
}

inline void Lexer::advanceState(const char c, const size_t i,
                                const BufHandle &bh,
                                LocalState &ss, Builder &parser)
{
   if (ss.substate_ != XSNone)
   {
      switch (ss.substate_)
      {
       case XSStartName:
         if (isnamestart(c))
         {
            ss.substate_ = XSInName;
            name_[ss.namepos_++] = c;
         }
         else
         {
            ss.substate_ = XSBad;
         }
         break;

       case XSInName:
         if (! isnamebody(c))
         {
            ss.substate_ = XSNone;
         }
         else
         {
            if (ss.namepos_ >= sizeof(name_))
            {
               throw_out_of_range();
            }
            name_[ss.namepos_++] = c;
         }
         break;

       case XSEntity:
         if (isnamestart(c))
         {
            ss.substate_ = XSNamedEntity;
         }
         else if (c == poundsign)
         {
            ss.substate_ = XSCharEntity;
         }
         else
         {
            ss.substate_ = XSBad;
         }
         break;

       case XSNamedEntity:
         if (c == semicolon)
         {
            ss.substate_ = XSEndEntity;
         }
         else if (!isnamebody(c))
         {
            ss.substate_ = XSBad;
         }
         break;

       case XSCharEntity:
         if (isdigit(c))
         {
            ss.substate_ = XSDecEntity;
         }
         else if (c == char_x)  // &#x
         {
            ss.substate_ = XSHexEntityStart;
         }
         else
         {
            ss.substate_ = XSBad;
         }

       case XSDecEntity:
         if (c == semicolon)
         {
            ss.substate_ = XSEndEntity;
         }
         else if (!isdigit(c))
         {
            ss.substate_ = XSBad;
         }
         break;

       case XSHexEntityStart:
         if (isxdigit(c))
         {
            ss.substate_ = XSHexEntity;
         }
         else
         {
            ss.substate_ = XSBad;
         }
         break;

       case XSHexEntity:
         if (c == semicolon)
         {
            ss.substate_ = XSEndEntity;
         }
         else if (!isxdigit(c))
         {
            ss.substate_ = XSBad;
         }
         break;

       case XSEndEntity:
         ss.substate_ = XSNone;
         break;

       case XSNone:
       case XSBad:
         break;

       default:
         throw_bad_case();
         break;
      }
   }
   if (ss.substate_ == XSBad)
   {
      ss.state_ = XBad;
   }
   else if (ss.substate_ == XSNone)
   {
      switch (ss.state_)
      {
       case XStart:
         if (c == lessthan)  // <
         {
            ss.elbegin_ = Position(bh, i);
            ss.used_elbegin_ = true;
            ss.state_ = XLess;
         }
         else if (! (getNonWSInElements() || iswhite(c)))
         {
            ss.state_ = XBad;
         }
         break;

       case XLess:
         if (c == exclamation)
         {
            ss.state_ = XCommentExcl;
            ss.used_elbegin_ = false;
         }
         else if (isnamestart(c))
         {
            ss.state_ = XOpenElement;
            ss.substate_ = XSInName;
            ss.namepos_ = 0;
            name_[ss.namepos_++] = c;
         }
         else if (c == forwslash)
         {
            ss.state_ = XCloseElement;
            ss.substate_ = XSStartName;
            ss.namepos_ = 0;
         }
         else
         {
            ss.state_ = XBad;
         }
         break;

       case XCommentExcl:
         if (c == dash)
         {
            ss.state_ = XCommentExclDash;
         }
         else
         {
            ss.state_ = XBad;
         }
         break;

       case XCommentExclDash:
         if (c == dash)
         {
            ss.state_ = XInComment;
         }
         else
         {
            ss.state_ = XBad;
         }
         break;

       case XInComment:
         if (c == dash)
         {
            ss.state_ = XDashInComment;
         }
         break;

       case XDashInComment:
         if (c == dash)
         {
            ss.state_ = XDashDashInComment;
         }
         else
         {
            ss.state_ = XInComment;
         }
         break;

       case XDashDashInComment:
         if (c == greaterthan)
         {
            ss.state_ = XStart;
         }
         else
         {
            ss.state_ = XInComment;
         }
         break;

       case XOpenElement:
         call_startElementTag(ss.elbegin_, ss.namepos_, parser);
         ss.used_elbegin_ = false;
         if (c == greaterthan)  // Parsed the whole tag
         {
            parser.endElementTag(Position(bh, i + 1), false);
            ss.state_ = XStart;
         }
         else if (iswhite(c))
         {
            ss.state_ = XInOpenElement;
         }
         else if (c == forwslash)
         {
            ss.state_ = XEmptyElementEnd;
         }
         else
         {
            ss.state_ = XBad;
         }
         break;

       case XInOpenElement:
         if (c == greaterthan) // Parsed the whole tag
         {
            parser.endElementTag(Position(bh, i + 1), false);
            ss.state_ = XStart;
         }
         else if (c == forwslash)
         {
            ss.state_ = XEmptyElementEnd;
         }
         else if (isnamestart(c))
         {
            ss.state_ = XAttr;
            ss.namepos_ = 0;
            name_[ss.namepos_++] = c;
            ss.attrbegin_ = Position(bh, i);
            ss.used_attr_ = true;
            ss.substate_ = XSInName;
         }
         else if (!iswhite(c))
         {
            ss.state_ = XBad;
         }
         break;

       case XEmptyElementEnd:
         if (c == greaterthan)
         {
            parser.endElementTag(Position(bh, i + 1), true);
            ss.state_ = XStart;  // Parsed the whole tag.
         }
         else
         {
            ss.state_ = XBad;
         }
         break;

       case XCloseElement:
         if (iswhite(c))
         {
            ss.state_ = XInCloseElement;
         }
         else if (c == greaterthan)
         {
            call_closeElementTag(ss.elbegin_, Position(bh, i + 1),
                                 ss.namepos_, parser);
            ss.used_elbegin_ = false;
            ss.state_ = XStart;  // Parsed the whole tag.
         }
         else
         {
            ss.state_ = XBad;
         }
         break;

       case XInCloseElement:
         if (c == greaterthan)
         {
            call_closeElementTag(ss.elbegin_, Position(bh, i + 1),
                                 ss.namepos_, parser);
            ss.used_elbegin_ = false;
            ss.state_ = XStart;  // Parsed the whole tag.
         }
         else if (!iswhite(c))
         {
            ss.state_ = XBad;
         }
         break;

       case XAttr:
         if (c == equals)
         {
            ss.state_ = XAttrAfterEq;
         }
         else if (!iswhite(c))
         {
            ss.state_ = XBad;
         }
         break;

       case XAttrAfterEq:
         if (c == doublequote)
         {
            ss.state_ = XAttrDQ;
            ss.attrvalbegin_ = Position(bh, i + 1);
            // Don't worry about recording that we're using this, because if
            // we set this value, attrbegin_ is ALWAYS set to an earlier
            // position.
         }
         else if (c == singlequote)
         {
            ss.state_ = XAttrSQ;
            ss.attrvalbegin_ = Position(bh, i + 1);
            // Don't worry about recording that we're using this, because if
            // we set this value, attrbegin_ is ALWAYS set to an earlier
            // position.
         }
         else if (!iswhite(c))
         {
            ss.state_ = XBad;
         }
         break;

       case XAttrSQ:
       case XAttrDQ:
         if (((ss.state_ == XAttrSQ) && (c == singlequote)) ||
             ((ss.state_ == XAttrDQ) && (c == doublequote)))
         {
            call_addAttribute(ss.attrbegin_, Position(bh, i + 1),
                              ss.attrvalbegin_, Position(bh, i),
                              ss.namepos_, parser);
            ss.used_attr_ = false;
            ss.state_ = XInOpenElement;
         }
         else if (c == lessthan)  // Don't ask me, amaya thinks it's evil.
         {
            ss.state_ = XBad;
         }
         break;

       case XBad:
         break;

       default:
         throw_bad_case();
         break;
      }
   }
}

bool Lexer::lex(const char *buf, unsigned int len,
                       BufHandle &lastbuf, Builder &parser)
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

} // namespace utf8
} // namespace xml
} // namespace strmod

// $Log$
// Revision 1.6  2003/01/10 02:27:04  hopper
// Moved stuff over from XMLUTF8Lexer.h because it was cluttering up the
// header flie.  Added in a couple of 'assert' style throws in impossible
// conditions.
//
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
