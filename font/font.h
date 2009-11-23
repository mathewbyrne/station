//
// font.h
//
// An Interface for a True Type C++ font class. The fonts themselves are
// rendered to OpenGL combatible textures and drawn as quads. This is a very
// basic implementation and perhaps not the most efficient way of rendering
// fonts in OpenGL.
//

#ifndef _FONT_H_
#define _FONT_H_


#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <string>
#include <vector>

using std::string;
using std::vector;


class Font
{

private:

  // Texture information.
  GLuint texId;
  int texW;
  int texH;

  // The first Display List for rendering.
  GLuint listStart;

  // Various font metics.
  int maxCharWidth;
  int maxCharHeight;
  int fontAscent;
  int fontDescent;
  int lineSpacing;
  int border;

  // Vector stores the width for each character.
  vector<int> charWidth;

  void getFontMetrics   ( TTF_Font *font );
  void makeFontTexture  ( TTF_Font *font );
  void makeDisplayLists ( void );

  static int fontCount;

public:

  Font( const string& fontPath, const int& size, const int& border = 0 );
  ~Font( void );

  void printStrLoc( const int& x, const int& y, const char *str ) const;
  void printStr( const char *str, const int& length ) const;

  void renderTexture( void ) const;

  static void setFontScreen( const int& width, const int& height );

};


#endif // _FONT_H_

