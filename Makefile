#
# Life Makefile
#
# Nothing complicated, just makes it all!
#

#
# Platform specific compiling options.
#
include config.os

CC        = g++
DEBUG     = -g
#PROFILE  = -pg
#OPTIMISE = -O3

PROGRAM = station
VERSION = 0.1

HEADERS = basegame.h station.h ltypes.h math/vec3.h math/matrix.h renderer.h \
					model/scene.h model/light.h model/camera.h material/shader.h \
					model/caster.h material/texture.h font/font.h global.h
OBJECTS = basegame.o station.o obj/grammar.tab.o obj/lexer.o obj/obj.o \
					model/model.o renderer.o model/camera.o material/shader.o \
					model/caster.o material/texture.o font/font.o

DEFINES = -DDEBUG
CFLAGS  = $(DEBUG) \
          $(PROFILE) \
          $(OPTIMISE) \
          $(PLATFORM_CFLAGS) \
          $(DEFINES)
LDFLAGS = $(PLATFORM_LIBS) -lpng -lz

EXE       = $(PROGRAM)$(PLATFORM_EXE)


all: $(EXE)

$(EXE) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE) $(LDFLAGS) 

$(OBJECTS): %.o: %.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

obj/grammar.tab.h: obj/grammar.tab.cpp

obj/grammar.tab.cpp: obj/grammar.y
	bison --defines=grammar.tab.h -v -o $@ $<

obj/lexer.cpp: obj/lexer.l
	flex -o$@ $<

clean:
	rm -f $(OBJECTS)
	rm -f $(EXE)

