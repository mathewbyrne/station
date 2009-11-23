//
// station.h
//
// Main header file for the whole application. Nothing interesting here...
//       ...just a bunch of includes and definitions...
//

#ifndef _STATION_H_
#define _STATION_H_


#include <vector>

#include "global.h"
#include "basegame.h"
#include "obj/obj.h"
#include "math/matrix.h"
#include "model/light.h"


class Camera;
class Scene;
class Renderer;


//
// Extension of the BaseGame class. No real need for this in the assignment
// but added a layer of abstraction between most of the SDL and OpenGL boiler
// plate style code.
//
class Station : public BaseGame
{

private:

	Camera *cam;
	Scene *scene;
	Renderer *renderer;

public:

  Station (void);
  ~Station (void);

  void render (void);
  void update (const uint& timePassed);
  void resize (const uint& newWidth, const uint& newHeight);

  void keyDown (const int& key);
  void keyUp (const int& key);
  void mouseMotion (const int& btn, const int& mx, const int& my);
  void mouseBtnUp (const int& btn, const int& mx, const int& my);
  void mouseBtnDown (const int& btn, const int& mx, const int& my);

};


#endif // _STATION_H_

