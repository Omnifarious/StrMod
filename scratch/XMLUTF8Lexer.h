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
   //! Just set up some initial defaults.
   XMLBuilder() : elmntok_(true),
                        wantattrs_(true), wantentities_(true) { }
   virtual ~XMLBuilder() {}

   virtual void startElementTag(size_t begin, const ::std::string &name) = 0;
   virtual void endElementTag(size_t end, bool wasempty) = 0;
   virtual void closeElementTag(size_t begin, size_t end,
                                const ::std::string &name) = 0;

   bool isElementOK() const { return elmntok_; }
   bool wantAttributes() const { return wantattrs_; }
   bool wantEntities() const { return wantentities_; }

 protected:
   bool elmntok_;
   bool wantattrs_;
   bool wantentities_;
};

class XMLUTF8Lexer
{
 private:
   enum XState { XBad, XStart, XLess,
                 XCommentExcl, XCommentExclDash, XInComment, XDashInComment,
                 XDashDashInComment,
                 XOpenElement, XInOpenElement, XEmptyElementEnd,
                 XCloseElement, XInCloseElement };

   enum XSubState { XSNone, XSBad, XSStartName, XSInName, XSEntity,
                    XSNamedEntity, XSCharEntity, XSDecEntity, XSHexEntityStart,
                    XSHexEntity, XSEndEntity };

 public:
   XMLUTF8Lexer()
        : state_(XStart), substate_(XSNone), namepos_(0), nonwsok_(false)
   {
   }

   bool getNonWSInElements() const                     { return nonwsok_; }
   void setNonWSInElements(bool nonwsok)               { nonwsok_ = nonwsok; }
   void lex(const char *buf, unsigned int len, XMLBuilder &parser);

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

   XState state_;
   XSubState substate_;
   char name_[256];
   size_t namepos_;
   bool nonwsok_;

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

   inline void advanceState(const char c)
   {
      if (substate_ != XSNone)
      {
         switch (substate_)
         {
          case XSStartName:
            if (isnamestart(c))
            {
               substate_ = XSInName;
               name_[namepos_++] = c;
            }
            else
            {
               substate_ = XSBad;
            }
            break;

          case XSInName:
            if (! isnamebody(c))
            {
               substate_ = XSNone;
            }
            else
            {
               name_[namepos_++] = c;
            }
            break;

          case XSEntity:
            if (isnamestart(c))
            {
               substate_ = XSNamedEntity;
            }
            else if (c == poundsign)
            {
               substate_ = XSCharEntity;
            }
            else
            {
               substate_ = XSBad;
            }
            break;

          case XSNamedEntity:
            if (c == semicolon)
            {
               substate_ = XSEndEntity;
            }
            else if (!isnamebody(c))
            {
               substate_ = XSBad;
            }
            break;

          case XSCharEntity:
            if (isdigit(c))
            {
               substate_ = XSDecEntity;
            }
            else if (c == char_x)  // &#x
            {
               substate_ = XSHexEntityStart;
            }
            else
            {
               substate_ = XSBad;
            }

          case XSDecEntity:
            if (c == semicolon)
            {
               substate_ = XSEndEntity;
            }
            else if (!isdigit(c))
            {
               substate_ = XSBad;
            }
            break;

          case XSHexEntityStart:
            if (isxdigit(c))
            {
               substate_ = XSHexEntity;
            }
            else
            {
               substate_ = XSBad;
            }
            break;

          case XSHexEntity:
            if (c == semicolon)
            {
               substate_ = XSEndEntity;
            }
            else if (!isxdigit(c))
            {
               substate_ = XSBad;
            }
            break;

          case XSEndEntity:
            substate_ = XSNone;
            break;

          case XSNone:
          case XSBad:
            break;

//        default:
//          throw ::std::logic_error("Bad case!");
         }
      }
      if (substate_ == XSBad)
      {
         state_ = XBad;
      }
      else if (substate_ == XSNone)
      {
         switch (state_)
         {
          case XStart:
            if (c == lessthan)  // <
            {
               state_ = XLess;
            }
            else if (! (getNonWSInElements() || iswhite(c)))
            {
               state_ = XBad;
            }
            break;

          case XLess:
            if (c == exclamation)
            {
               state_ = XCommentExcl;
            }
            else if (isnamestart(c))
            {
               state_ = XOpenElement;
               substate_ = XSInName;
               namepos_ = 0;
               name_[namepos_++] = c;
            }
            else if (c == forwslash)
            {
               state_ = XCloseElement;
               substate_ = XSStartName;
               namepos_ = 0;
            }
            else
            {
               state_ = XBad;
            }
            break;

          case XCommentExcl:
            if (c == dash)
            {
               state_ = XCommentExclDash;
            }
            else
            {
               state_ = XBad;
            }
            break;

          case XCommentExclDash:
            if (c == dash)
            {
               state_ = XInComment;
            }
            else
            {
               state_ = XBad;
            }
            break;

          case XInComment:
            if (c == dash)
            {
               state_ = XDashInComment;
            }
            break;

          case XDashInComment:
            if (c == dash)
            {
               state_ = XDashDashInComment;
            }
            else
            {
               state_ = XInComment;
            }
            break;

          case XDashDashInComment:
            if (c == greaterthan)
            {
               state_ = XStart;
            }
            else
            {
               state_ = XInComment;
            }
            break;

          case XOpenElement:
            if (c == greaterthan)  // Parsed the whole tag
            {
               state_ = XStart;
            }
            else if (iswhite(c))
            {
               state_ = XInOpenElement;
            }
            else if (c == forwslash)
            {
               state_ = XEmptyElementEnd;
            }
            else
            {
               state_ = XBad;
            }
            break;

          case XInOpenElement:
            if (c == greaterthan) // Parsed the whole tag
            {
               state_ = XStart;
            }
            else if (c == forwslash)
            {
               state_ = XEmptyElementEnd;
            }
            else if (!iswhite(c))
            {
               state_ = XBad;
            }
            break;

          case XEmptyElementEnd:
            if (c == greaterthan)
            {
               state_ = XStart;  // Parsed the whole tag.
            }
            else
            {
               state_ = XBad;
            }
            break;

          case XCloseElement:
            if (iswhite(c))
            {
               state_ = XInCloseElement;
            }
            else if (c == greaterthan)
            {
               state_ = XStart;  // Parsed the whole tag.
            }
            else
            {
               state_ = XBad;
            }
            break;

          case XInCloseElement:
            if (c == greaterthan)
            {
               state_ = XStart;  // Parsed the whole tag.
            }
            else if (!iswhite(c))
            {
               state_ = XBad;
            }
            break;

          case XBad:
            break;
         }
      }
   }

   static void throw_out_of_range();
   void call_startElementTag(size_t begin, XMLBuilder &parser);
   void call_closeElementTag(size_t begin, size_t end, XMLBuilder &parser);
};

// $Log$
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
