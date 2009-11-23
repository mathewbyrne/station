# $Id: Makefile,v 1.2 2006/09/01 13:46:58 mbyrne Exp $

CPPFLAGS += -g -Wall
LDFLAGS += -lGL -lGLU -lglut

OBJECTS=grammar.tab.o lexer.o obj.o viewobj.o ../model/model.o
HEADERS=../math/vec3.h ../model/model.h ../model/light.h obj.h

viewobj: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): %.o: %.cpp $(HEADERS)
	g++ -c $(CPPFLAGS) $< -o $@

grammar.tab.h: grammar.tab.cpp

grammar.tab.cpp: grammar.y
	bison --defines=grammar.tab.h -v -o $@ $<

lexer.cpp: lexer.l
	flex -o$@ $<

clean: 
	rm -f grammar.tab.h grammar.tab.cpp grammar.output \
	      lexer.cpp \
		  $(OBJECTS) $(LIBOBJ) \
		  viewobj $(VIEW_OBJECTS)
