//
// font.cpp
//
// Font class implementation.
//

#include "font.h"
#include <stdexcept>

#ifdef DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif


int Font::fontCount = 0;


//
// Creates a new font object and prepares it for OpenGL rendering. size
// specifies the pt size of the font, border specifies a specific border
// around each character.
//
Font::Font( const string& filename, const int& size, const int& border )
  : texId( 0 ), texW( 1 ), texH( 1 ), listStart( 0 ),
    maxCharWidth( 0 ), maxCharHeight( 0 ), fontAscent( 0 ),
    fontDescent( 0 ), lineSpacing( 0 ), border( border )
{

  if( !fontCount )
  {
    if( TTF_Init() < 0 )
      throw std::runtime_error( "Error initializing SDL_ttf." );
  }
  fontCount++;
  
  TTF_Font *font = TTF_OpenFont( filename.c_str(), size );

  if( !font )
  {
    string error = "Unable to load font file '" + filename + "'";
    throw std::runtime_error( error.c_str() );
  }

  getFontMetrics( font );
  makeFontTexture( font );
  makeDisplayLists();

  TTF_CloseFont( font );

#ifdef DEBUG
  cout << "Font '" << filename << "' succesfully loaded." << endl
       << "  size       : " << size << endl
       << "  border     : " << border << endl
       << "  tex-width  : " << texW << endl
       << "  tex-height : " << texH << endl
       << "  font-asc   : " << fontAscent << endl
       << "  font-desc  : " << fontDescent << endl
       << "  max-width  : " << maxCharWidth << endl
       << "  max-height : " << maxCharHeight << endl
       << "  list-start : " << listStart << endl;
#endif

}


//
// Clears all the Display Lists used by this particular Font instance, and
// deletes texture resources if any have been used.
//
Font::~Font( void )
{
  if( glIsTexture( texId ) )
    glDeleteTextures( 1, &texId );

  glDeleteLists( listStart, 256 );

  fontCount--;
  if( !fontCount )
    TTF_Quit();
}


//
// Prints a string of text at a set location. Note that unless specifically
// set up, this is not neccesarily a screen coordinate location.
//
void Font::printStrLoc( const int& x, const int& y, const char *str ) const
{
  glLoadIdentity();
  glTranslatef( x, y, 0.0 );

  // Save OpenGL state and alter it to suit our needs.
  glPushAttrib( GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT );
  glDisable( GL_LIGHTING );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, texId );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  // This loop seperates lines of text.
  const char *start = str;
  while( start )
  {
    int length;

    char *end = strchr( start, '\n' );
    if( !end )
      length = strlen( start );
    else
    {
      end++;
      length = end - start - 1;
    }

    // Print an individual line of text.
    glPushMatrix();
      printStr( start, length );
    glPopMatrix();

    // Move to the next line.
    glTranslatef( 0.0, -lineSpacing, 0.0 );

    start = end;
  }

  // Restore the state to before this function.
  glPopAttrib();
}


//
// Uses a set of display lists to print a single string up to the supplied
// length.
//
void Font::printStr( const char *str, const int& length ) const
{
  glListBase( listStart );
  glCallLists( length, GL_UNSIGNED_BYTE, str );
}


//
// Mostly for testing purposes, outputs the entire font texture to a quad on
// the screen.
//
void Font::renderTexture( void ) const
{
  glPushAttrib( GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT );

  glDisable( GL_LIGHTING );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, texId );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  glBegin( GL_QUADS );
    glTexCoord2f( 1.0, 1.0 ); glVertex2f( texW, 0.0 );
    glTexCoord2f( 1.0, 0.0 ); glVertex2f( texW, texH );
    glTexCoord2f( 0.0, 0.0 ); glVertex2f(  0.0, texH );
    glTexCoord2f( 0.0, 1.0 ); glVertex2f(  0.0, 0.0 );
  glEnd();

  glPopAttrib();
}


//
// Gets the metrics of the font that are specified in the TTF file itself.
// These are used for general text layout.
//
void Font::getFontMetrics( TTF_Font *font )
{
  charWidth.reserve( 256 );
  maxCharWidth = 0;

  for( Uint16 i = 0; i < 256; i++ )
  {
    int doomed, tempCharWidth;

    TTF_GlyphMetrics( font, i, &doomed, &doomed, &doomed, &doomed,
        &tempCharWidth );
    charWidth.push_back( tempCharWidth );

    if( tempCharWidth > maxCharWidth )
      maxCharWidth = tempCharWidth;
  }

  maxCharHeight = TTF_FontHeight( font );
  fontDescent   = TTF_FontDescent( font );
  fontAscent    = TTF_FontAscent( font );
  lineSpacing   = TTF_FontLineSkip( font );
}


//
// Creates the actual texture that will be used to draw our characters from.
//
void Font::makeFontTexture( TTF_Font *font )
{
  // Work out the minimum dimensions needed for the texture.
  int width  = ( maxCharWidth  + border ) * 16 + border;
  int height = ( maxCharHeight + border ) * 16 + border;

  // Find the next greatest power of 2 for each width and height.
  while( texW < width )
    texW = texW << 1;
  while( texH < height )
    texH = texH << 1;

  // Create a new texture and set it's entire color to black with no alpha.
  int texSize = texW * texH * 4;
  GLubyte *texels = new GLubyte[ texSize ];
  memset( texels, 0, texSize );

  glGenTextures( 1, &( texId ) );
  glBindTexture( GL_TEXTURE_2D, texId );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glTexImage2D( GL_TEXTURE_2D, 0, 4, texW, texH, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, texels );

  delete[] texels;

  int xOff, yOff;
  xOff = yOff = border;

  // Next create the actual character set.
  for( int i = 0; i < 16; i++ )
  {
    for( int j = 0; j < 16; j++ )
    {
      // First create a null terminated string from the desired char.
      char letter[ 2 ];
      letter[ 0 ] = static_cast<char>( i * 16 + j );
      letter[ 1 ] = '\0';

      if( letter[0] )
      {
        // Rendered in white.
        SDL_Color forecolor = { 255, 255, 255, 255 };
        SDL_Surface *lSurface = TTF_RenderText_Blended( font, letter,
            forecolor );

        if( lSurface )
        {
          glTexSubImage2D( GL_TEXTURE_2D, 0, xOff, yOff, lSurface->w,
              lSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, lSurface->pixels );
          SDL_FreeSurface( lSurface );
        }
      }

      xOff += maxCharWidth + border;
    }

    xOff  = border;
    yOff += maxCharHeight + border;
  }

}


//
// Generates a set of Display Lists that can be used to print the individual
// characters to the screen.
//
void Font::makeDisplayLists( void )
{
  listStart = glGenLists( 256 );

  GLfloat tw = static_cast<float>( texW );
  GLfloat th = static_cast<float>( texH );
  int count  = 0;
  GLfloat tHeight = ( fontAscent - fontDescent ) / th;

  GLfloat s = border / tw;
  GLfloat t = border / th;

  for( int i = 0; i < 16; i++ )
  {

    for( int j = 0; j < 16; j++, count++ )
    {
      // Difference between s texture coordinates on the Quad.
      GLfloat sWidth = charWidth[ count ] / tw;

      glNewList( listStart + count, GL_COMPILE );

        glBegin(GL_QUADS);
          glTexCoord2f( s, t + tHeight );
          glVertex2i( 0, fontDescent );
          glTexCoord2f( s + sWidth, t + tHeight );
          glVertex2i( charWidth[ count ], fontDescent );
          glTexCoord2f( s + sWidth, t );
          glVertex2i( charWidth[ count ], fontAscent );
          glTexCoord2f( s, t );
          glVertex2i( 0, fontAscent );
        glEnd();

        // Move onto the next character.
        glTranslatef( charWidth[ count ], 0.0, 0.0 );

      glEndList();

      s += ( maxCharWidth + border ) / tw;
    }

    s = border / tw;
    t += ( maxCharHeight + border ) / th;
  }
}


//
// Creates a display list which sets up the screen in such a way that the
// coordinate system matches screen pixels. In other words, width and
// height should be the size of the Viewport in pixels.
//
void Font::setFontScreen( const int& width, const int& height )
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0, width, 0, height );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

