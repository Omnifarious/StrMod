# $URL$
# $Author$
# $Date$
# $Rev$

CXX = g++ -pipe -march=pentium3
CPPFLAGS = -I ../include
CXXFLAGS = -Wall -O2
CXXSUPER = -Wall -O3 -finline-limit=50000
#CXXFLAGS = -Wall -g
#CXXSUPER = -Wall -g

all : superopt testparse

clean:
	rm -f testparse *.o

superopt:
	$(MAKE) CXXFLAGS="$(CXXSUPER)" utf8_Lexer.o

.PHONY: all clean superopt

testparse : utf8_Lexer.o utf8_LexerSane.o XMLUTF8Parser.o utf8_Builder.o
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@


utf8_Lexer.o : xml/utf8/Lexer.h xml/utf8/Builder.h

#	$(CXX) -c $(CPPFLAGS) $(CXXSUPER) -c $< -o $@

#XMLUTF8LexerSane.o : XMLUTF8LexerSane.cxx XMLUTF8Lexer.h
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#XMLUTF8Parser.o : XMLUTF8Parser.cxx XMLUTF8Lexer.h
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

utf8_LexerSane.o : xml/utf8/Lexer.h xml/utf8/Builder.h

XMLUTF8Parser.o : xml/utf8/Lexer.h xml/utf8/Builder.h

utf8_Builder.o : xml/utf8/Builder.h

# $Log$
# Revision 1.3  2002/12/10 22:46:02  hopper
# Renamed the XMLParserStrategy to the more appropriate XMLBuilder from
# Design Patterns.
#
# Revision 1.2  2002/12/10 13:20:18  hopper
# Put comments in Makefile comment format instead of C++.  Oops.
#
# Revision 1.1  2002/12/10 13:19:16  hopper
# Some XML parsing classes I'm playing with until I get something that's
# suitable for public consumption.
