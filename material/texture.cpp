//
// texture.cpp
//
// SDL_image to OpenGL texture loading.
//

#include "texture.h"

#include <SDL/SDL.h>
#include <OpenGL/gl.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <stdexcept>


//
// Loads a texture of any format supported by the SDL_image IMG_Load function.
// Tends to load things upside down but my programs tend to work that way
// anyhow.
//
void Texture::loadTexture( const char *filename )
{
  // Free any currently assigned texture.
  if( texId && glIsTexture( texId ) )
  {
    glDeleteTextures( 1, &texId );
    texId = 0;
  }

  SDL_Surface *surface;
  GLint mode;

  // Actually load the image.
  surface = IMG_Load( filename );
  if( !surface )
    throw std::runtime_error( "Unable to load texture." );

  // Determine the bit format.
  if( surface->format->BytesPerPixel == 3 )
    mode = GL_RGB;
  else if( surface->format->BytesPerPixel == 4 )
    mode = GL_RGBA;
  else
  {
    SDL_FreeSurface( surface );
    throw std::runtime_error( "Unable to determine image bit-depth." );
  }

  // Do the OpenGL stuff here.
  glGenTextures( 1, &texId );
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texId );

  glTexImage2D( GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode,
      GL_UNSIGNED_BYTE, surface->pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  SDL_FreeSurface( surface );
}


//
// Deletes any OpenGL texture resources.
//
Texture::~Texture( void )
{
  if( texId && glIsTexture( texId ) )
    glDeleteTextures( 1, &texId );
}


//
// Bind the current texture. Throws an exception if no texture is currently
// loaded.
//
void Texture::bindTexture( void ) const
{
  if( texId )
    glBindTexture( GL_TEXTURE_2D, texId );
  else
    throw std::runtime_error( "No texture loaded." );
}

