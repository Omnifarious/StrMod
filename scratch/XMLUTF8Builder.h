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

/**
 */
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

#endif

// $Log$
