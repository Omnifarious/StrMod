/* $Header$ */

CXX = g++ -pipe -march=athlon-xp
CXXFLAGS = -Wall -O2
CXXSUPER = -Wall -O3 -finline-limit=50000
#CXXFLAGS = -Wall -g
#CXXSUPER = -Wall -g

all : superopt testparse

clean:
	rm -f testparse *.o

superopt:
	$(MAKE) CXXFLAGS="$(CXXSUPER)" XMLUTF8Lexer.o

.PHONY: all clean superopt

testparse : XMLUTF8Lexer.o XMLUTF8LexerSane.o XMLUTF8Parser.o
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@


XMLUTF8Lexer.o : XMLUTF8Lexer.h

#	$(CXX) -c $(CPPFLAGS) $(CXXSUPER) -c $< -o $@

#XMLUTF8LexerSane.o : XMLUTF8LexerSane.cxx XMLUTF8Lexer.h
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#XMLUTF8Parser.o : XMLUTF8Parser.cxx XMLUTF8Lexer.h
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

XMLUTF8LexerSane.o : XMLUTF8Lexer.h

XMLUTF8Parser.o : XMLUTF8Lexer.h

// $Log$
// Revision 1.1  2002/12/10 13:19:16  hopper
// Some XML parsing classes I'm playing with until I get something that's
// suitable for public consumption.
//
