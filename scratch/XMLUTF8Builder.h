#ifndef _XML_XMLUTF8Builder_H_  // -*-c++-*-

#define _XML_XMLUTF8Builder_H_

#ifdef __GNUG__
#  pragma interface
#endif

/* $URL$
 * $Author$
 * $Date$
 * $Rev$
 */

#include <string>
#include <cstddef>

/** \class XMLBuilder XMLUTF8Builder.h XMLUTF8Builder.h
 * An interface class for an XMLUTF8Lexer to use to put tokens together into
 * some other structure.
 *
 * This is so the XMLUTF8Lexer doesn't have to know the details of how the
 * parser (or whatever is interpreting the output of the XMLUTF8Lexer) works.
 * It follows the Builder pattern from Design Patterns.
 */
class XMLBuilder
{
 public:
   //! Handle for positions from previous calls to XMLUTF8Lexer::lex
   union BufHandle {
      unsigned long ulval_;
      void *ptrval_;
   };
   /**
    * The position of a token start, end, or other feature.
    *
    * Since the XMLUTF8Lexer works on character buffers, and a given token
    * may extend across several buffers, it also needs a way to communicate
    * which buffer a particular token started or ended in.  To this end, since
    * the XMLUTF8Lexer reports the positions in the buffer of token beginnings
    * and endings, it reports those positions using an identifier (i.e. handle)
    * for which buffer the position was in, what the offset is from that
    * buffer.
    *
    * This whole system is designed with three goals:
    * - Reduce the amount of information that has to be copied.</li>
    * - Open up as much code to the optimizer as possible.</li>
    * - Make sure each character in an XML document is only processed once
    *    on the way to a parse tree.
    */
   struct Position {
      BufHandle bufhdl_;
      size_t bufoffset_;

      Position(const BufHandle &bufhdl, const size_t &bufoffset)
           : bufhdl_(bufhdl), bufoffset_(bufoffset)
      {
      }
      Position() : bufoffset_(0) { bufhdl_.ulval_ = 0; }
   };

   //! It's an interface, so this doesn't do anything.
   XMLBuilder() { }
   //! It's an interface, so this doesn't do anything.
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

#endif

// $Log$
