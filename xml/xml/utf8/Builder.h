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

namespace strmod {
namespace xml {
namespace utf8 {

/** \class Builder Builder.h xml/utf8/Builder.h
 * An interface class for an Lexer to use to put tokens together into some other
 * structure.
 *
 * This is so the Lexer doesn't have to know the details of how the parser (or
 * whatever is interpreting the output of the Lexer) works.  It follows the
 * Builder pattern from Design Patterns.
 *
 * In the various member functions that follow, there are various parameters
 * describing the positions of things.  In order to make this description
 * clearer, here is a diagram:
 * <pre>
 * &lt;LongTagName withan="attribute">And some element text&lt;/LongTagName> 
 * ^            ^       ^        ^^^                    ^             ^
 * |            |       |        |||                    |             |
 * `->selbegin  |       |        ||`->selend  celbegin<-'     celend<-'
 *              |       |        ||
 *   attrbegin<-'       |        |`->attrend
 *                      |        |
 *            valbegin<-'        `->valend
 * </pre>
 */
class Builder
{
 public:
   /** \class BufHandle Builder.h xml/utf8/Builder.h
    * Handle for positions from previous calls to Lexer::lex
    */
   union BufHandle {
      unsigned long ulval_;  //!< If the user uses an integral type for the handle.
      void *ptrval_; //!< If the user uses a pointer type for the handle.
   };
   /** \class Position Builder.h xml/utf8/Builder.h
    * The position of a token start, end, or other feature.
    *
    * Since the Lexer works on character buffers, and a given token may extend
    * across several buffers, it also needs a way to communicate which buffer a
    * particular token started or ended in.  To this end, since the Lexer
    * reports the positions in the buffer of token beginnings and endings, it
    * reports those positions using an identifier (i.e. handle) for which buffer
    * the position was in, what the offset is from that buffer.
    *
    * An offset may point one past the last achracter in the buffer identified
    * by the handle.
    *
    * This whole system is designed with three goals:
    * - Reduce the amount of information that has to be copied.
    * - Open up as much code to the optimizer as possible.
    * - Make sure each character in an XML document is only processed once
    *    on the way to a parse tree.
    */
   struct Position {
      //! A buffer identifier passed into Lexer::lex
      BufHandle bufhdl_;
      //! The offset from the beginning of the buffer represented by bufhdl_
      // This may point one past the last achracter in the buffer identified by
      // the handle.
      size_t bufoffset_;

      //! Convenience initializing contructor
      Position(const BufHandle &bufhdl, const size_t &bufoffset)
           : bufhdl_(bufhdl), bufoffset_(bufoffset)
      {
      }
      //! Give things reasonable default values.
      Position() : bufoffset_(0) { bufhdl_.ulval_ = 0; }
   };

   //! It's an interface, so this doesn't do anything.
   Builder() = default;
   //! It's an interface, so this doesn't do anything.
   virtual ~Builder() = default;

   /** The lexer encountered an element open tag, atributes may follow
    *
    * @param selbegin The buffer position of the '<' of the tag.
    * @param name The name of the element being opened.
    */
   virtual void startElementTag(const Position &selbegin,
                                const ::std::string &name) = 0;
   /** The lexer encountered an attribute of an element open tag.
    *
    * @param attrbegin	The buffer position of the first character of the tag name.
    *
    * @param attrend 	The buffer position one past the closing single
    *     or double quote of the attribute value.
    *
    * @param valbegin	The buffer position of the first character after
    *     the opening single or double quote of the attribute value.
    *
    * @param valend	The buffer position of the closing single or
    *     double quote of the attribute value.
    *
    * @param name	The attribute name.
    */
   virtual void addAttribute(const Position &attrbegin, const Position &attrend,
                             const Position &valbegin, const Position &valend,
                             const ::std::string &name) = 0;
   /** The lexer encountered the closing '>' of an element open tag.
    *
    * @param selend The buffer position one past the '>'.
    * @param wasempty Was the tag of the form &lt;br/>?
    */
   virtual void endElementTag(const Position &selend, bool wasempty) = 0;
   /** The lexer encountered the close element tag (a tag of the form &lt;/p>
    *
    * @param celbegin The buffer position of the '<'.
    * @param celend The buffer position one past the '>'.
    * @param name The name of the element being closed.
    */
   virtual void closeElementTag(const Position &celbegin,
                                const Position &celend,
                                const ::std::string &name) = 0;
};

} // namespace utf8
} // namespace xml
} // namespace strmod

#endif
