#ifndef _XML_XMLUTF8Lexer_H_  // -*-c++-*-

#define _XML_XMLUTF8Lexer_H_

/* $Header$ */

#ifdef __GNUG__
#  pragma interface
#endif

#include <cstddef>
#include <string>

class XMLBuilder
{
 public:
   //! Handle for positions from previous calls to XMLUTF8Lexer::lex
   union BufHandle {
      unsigned long ulval_;
      void *ptrval_;
   };
   struct Position {
      BufHandle bufhdl_;
      size_t bufoffset_;

      Position(const BufHandle &bufhdl, const size_t &bufoffset)
           : bufhdl_(bufhdl), bufoffset_(bufoffset)
      {
      }
      Position() : bufoffset_(0) { bufhdl_.ulval_ = 0; }
   };

   //! Just set up some initial defaults.
   XMLBuilder() { }
   virtual ~XMLBuilder() {}

   virtual void startElementTag(const Position &begin,
                                const ::std::string &name) = 0;
   virtual void addAttribute(const Position &attrbegin, const Position &attrend,
                             const Position &valbegin, const Position &valend,
                             const ::std::string &name) = 0;
   virtual void endElementTag(const Position &end, bool wasempty) = 0;
   virtual void closeElementTag(const Position &begin, const Position &end,
                                const ::std::string &name) = 0;
};

class XMLUTF8Lexer
{
 private:
   enum XState { XBad, XStart, XLess,
                 XCommentExcl, XCommentExclDash, XInComment, XDashInComment,
                 XDashDashInComment,
                 XOpenElement, XInOpenElement, XEmptyElementEnd,
                 XCloseElement, XInCloseElement,
                 XAttr, XAttrAfterEq, XAttrSQ, XAttrDQ };

   enum XSubState { XSNone, XSBad,
                    XSStartName, XSInName,
                    XSEntity, XSNamedEntity, XSCharEntity, XSDecEntity,
                    XSHexEntityStart, XSHexEntity, XSEndEntity };
   typedef XMLBuilder::BufHandle BufHandle;
   typedef XMLBuilder::Position Position;

 public:
   XMLUTF8Lexer()
        : nonwsok_(false)
   {
   }

   //! Does the lexer consider non-whitespace inside of elements (between tags) to be an error?
   bool getNonWSInElements() const                     { return nonwsok_; }
   //! Tell the lexer whether or not non-whitespace inside of elements (between tags) is an error.
   void setNonWSInElements(bool nonwsok)               { nonwsok_ = nonwsok; }

   /** Process the UTF-8 encoded characters in buf, calling the builder at the appropriate points.
    * @param buf A pointer to an array of characters to process.
    * @param len The number of characters in the array pointed to by buf.
    * @param lastbuf IN: A buffer handle to identify which call of lex a particular token started in / OUT: The earlist buffer handle still being used internally by the XMLUTF8Lexer.
    * @param builder The builder to call when tokens are encountered.
    * @return true if XMLUTF8Lexer is storing no BufHandles, and lastbuf contains a valid BufHandle, false if it isn't.
    */
   bool lex(const char *buf, unsigned int len,
            BufHandle &lastbuf, XMLBuilder &builder);

 private:
   static const char exclamation = '\x21';
   static const char doublequote = '\x22';
   static const char poundsign = '\x23';
   static const char singlequote = '\x27';
   static const char ampersand = '\x26';
   static const char dash = '\x2D';
   static const char dot = '\x2E';
   static const char forwslash = '\x2F';
   static const char colon = '\x3A';
   static const char semicolon = '\x3B';
   static const char lessthan = '\x3C';
   static const char equals = '\x3D';
   static const char greaterthan = '\x3E';
   static const char char_x = '\x78';
   static const ::std::string out_of_range_message;

   char name_[256];
   bool nonwsok_;
   struct LocalState {
      XState state_;
      XSubState substate_;
      size_t namepos_;
      bool used_elbegin_, used_attr_;
      Position elbegin_, attrbegin_, attrvalbegin_;

      LocalState()
           : state_(XStart), substate_(XSNone), namepos_(0),
             used_elbegin_(false), used_attr_(false)
      {
      }
   } localstate_;

   static inline bool iswhite(const char c)
   {
      return
         (c == '\x20') || // Space in UTF8, ' ' depends on encoding used by C compiler.
         (c == '\x9') ||  // Tab
         (c == '\xA') || // newline
         (c == '\xD');  // carraige return
   }

   static inline bool isnamestart(const char c)
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

   static inline bool isnamebody(const char c)
   {
      return
         isnamestart(c) ||
         (c >= '\x30') && (c <= '\x39') || // 0-9
         (c == dot) || (c == dash) ||
         (c == '\xB7');  // Extender, whatever that is.
   }

   static inline bool isdigit(const char c)
   {
      return (c >= '\x30') && (c <= '\x39');  // 0-9
   }

   static inline bool isxdigit(const char c)
   {
      return
         isdigit(c) ||
         ((c >= '\x41') && (c <= '\x46')) || // A-F
         ((c >= '\x61') && (c <= '\x66'));
   }

   inline void advanceState(const char c, const size_t i, const BufHandle &bh,
                            LocalState &ss, XMLBuilder &parser)
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

//        default:
//          throw ::std::logic_error("Bad case!");
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
         }
      }
   }

   static void throw_out_of_range();
   void call_startElementTag(const Position &begin, size_t namepos,
                             XMLBuilder &parser);
   void call_addAttribute(const Position &attrbegin, const Position &attrend,
                          const Position &valbegin, const Position &valend,
                          size_t namepos, XMLBuilder &parser);
   void call_closeElementTag(const Position &begin, const Position &end,
                             size_t namepos, XMLBuilder &parser);
};

// $Log$
// Revision 1.11  2003/01/09 22:48:32  hopper
// Much farter along multiple buffer parsing.
//
// Revision 1.10  2003/01/09 03:43:52  hopper
// Farther along the path to a decent XML parser.
//
// Revision 1.9  2003/01/08 18:00:07  hopper
// More scrabblings along the path to a decent XML parser.
//
// Revision 1.8  2003/01/06 15:18:35  hopper
// Towards further attempts to record positions in previously parsed buffers.
//
// Revision 1.7  2002/12/11 21:55:41  hopper
// It parses attributes now.  There's even a decent test for it.  :-)
//
// Revision 1.6  2002/12/11 18:52:02  hopper
// More steps towards parsing attributes.
//
// Revision 1.5  2002/12/11 13:42:36  hopper
// Moving towards handling attributes, and multi-buffer parsing.
//
// Revision 1.4  2002/12/10 22:46:02  hopper
// Renamed the XMLParserStrategy to the more appropriate XMLBuilder from
// Design Patterns.
//
// Revision 1.3  2002/12/10 16:08:42  hopper
// Preliminary changes to allow elements to have #PCDATA.
//
// Revision 1.2  2002/12/10 13:21:13  hopper
// Moved Header line to better place.
//
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
//

#endif
