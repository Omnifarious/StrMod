#ifndef _XML_XMLUTF8Lexer_H_  // -*-c++-*-

#define _XML_XMLUTF8Lexer_H_

/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */


#include <xml/utf8/Builder.h>
#include <cstddef>
#include <string>

namespace strmod {
namespace xml {
namespace utf8 {

/** \class Lexer Lexer.h xml/utf8/Lexer.h
 * Finds tokens and reports them, and their positions to a Builder.
 */
class Lexer
{
 public:
   //! Constructs a Lexer in the between tag, not in a comment state.
   Lexer()
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
    * @param lastbuf IN: A buffer handle to identify which call of lex a particular token started in / OUT: The earlist buffer handle still being used internally by the Lexer.
    * @param builder The builder to call when tokens are encountered.
    * @return true if Lexer is storing no BufHandles, and lastbuf contains a valid BufHandle, false if it isn't.
    */
   bool lex(const char *buf, unsigned int len,
            Builder::BufHandle &lastbuf, Builder &builder);

 private:
   typedef Builder::Position Position;
   typedef Builder::BufHandle BufHandle;

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
   static const ::std::string bad_case_message;

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

   static inline bool iswhite(const char c);
   static inline bool isnamestart(const char c);
   static inline bool isnamebody(const char c);
   static inline bool isdigit(const char c);
   static inline bool isxdigit(const char c);

   inline void advanceState(const char c, const size_t i, const BufHandle &bh,
                            LocalState &ss, Builder &parser);

   static void throw_out_of_range();
   static void throw_bad_case();
   void call_startElementTag(const Position &begin, size_t namepos,
                             Builder &parser);
   void call_addAttribute(const Position &attrbegin, const Position &attrend,
                          const Position &valbegin, const Position &valend,
                          size_t namepos, Builder &parser);
   void call_closeElementTag(const Position &begin, const Position &end,
                             size_t namepos, Builder &parser);
};

} // namespace utf8
} // namespace xml
} // namespace strmod

// $Log$
// Revision 1.12  2003/01/10 02:28:37  hopper
// Moved stuff over to XMLUTF8Lexer.cpp from XMLUTF8Lexer.h because it was
// cluttering up the header file.  Added a function to throw an exception
// when the main lexer routine encounters 'impossible' conditions.
//
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
