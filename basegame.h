//
// basegame.h
//
// Basic game framework for an OpenGL/SDL program. The class below is abstract
// and should be overwritten. Provides a basic event handling loop and abstract
// functions for input and other event handling.
//

#ifndef _BASEGAME_H_
#define _BASEGAME_H_


#include "ltypes.h"
#include <string>
#include <SDL/SDL.h>

using namespace std;


static const uint DEF_WIDTH  = 800;
static const uint DEF_HEIGHT = 600;


class BaseGame
{

protected:

  SDL_Surface *screen;

  ulong  lastTime;
  ulong  thisTime;

  uint winWidth;
  uint winHeight;

  string winTitle;

  bool   running;

  int    fpsTicks;
  int    fpsCounter;
  float  fpsCurrent;

  void pUpdate( void );
  void pRender( void );
  void pEvents( void );

  uint flags;


public:

  BaseGame( const uint& width = DEF_WIDTH, const uint& height = DEF_HEIGHT,
      const uint& flags = SDL_OPENGL | SDL_RESIZABLE );
  virtual ~BaseGame( void );

  // Starts the engine.
  void start( void );

  // Public versions of the protected functions.
  virtual void render( void )    = 0;
  virtual void update( const uint& timePassed ) = 0;
  virtual void resize( const uint& newWidth, const uint& newHeight ) = 0;

  // Input handling functions.
  virtual void keyDown( const int& key ) = 0;
  virtual void keyUp  ( const int& key ) = 0;
  virtual void mouseMotion ( const int& btn, const int& mx, const int& my ) = 0;
  virtual void mouseBtnUp  ( const int& btn, const int& mx, const int& my ) = 0;
  virtual void mouseBtnDown( const int& btn, const int& mx, const int& my ) = 0;

  // Misc mutators/accessors.
  void setTitle( const string& title );

  SDL_Surface  *getScreen() const;
  const string& getTitle()  const;
  const float&  getFps()    const;

};


#endif // _BASEGAME_H_
