#
# Linux configuration
#

PLATFORM_LIBS   = `sdl-config --libs` -lSDL_image -lSDL_ttf -framework Cocoa -framework OpenGL -lSDLmain
PLATFORM_CFLAGS = `sdl-config --cflags`
PLATFORM_EXE    = 
