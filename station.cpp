//
// station.cpp
//
// Implementation of the main program method. Also contains the Station class
// implementation. Inside this class, all input, coordination and events
// occur. Since this is a smaller application, these tasks are all handled here
// rather than delegated to more specialised classes.
//


#include "station.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


#include "renderer.h"
#include "model/camera.h"
#include "model/scene.h"
#include "obj/obj.h"


// Urgh...     ...anyway
static ObjModel *interior, *cube, *sphere, *torus;


Station::Station()
{
	// Instantiate all the classes required for the application.
	cam      = new Camera(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0));
	scene    = new Scene();
	renderer = new Renderer();

  // All the required models are loaded here for placement in the scene. Each
  // model only contains information that is relevant to all Casters that use
  // a particular model.
  interior = new ObjModel("data/models/interior.obj");
	cube     = new ObjModel("data/models/cube.obj");
  sphere   = new ObjModel("data/models/cylinder-nn.obj");
  torus    = new ObjModel("data/models/ring.obj");

  cube    ->initVertexBuffers();
  sphere  ->initVertexBuffers();
  torus   ->initVertexBuffers();
  interior->initVertexBuffers();

  scene->addCaster(Caster(cube,   Vec3( 0.0,  2.0,  0.0), Vec3()));
  scene->addCaster(Caster(cube,   Vec3( 4.0,  3.0,  4.0), Vec3()));
  scene->addCaster(Caster(sphere, Vec3(-3.0,  2.0, -2.0), Vec3()));
  scene->addCaster(Caster(sphere, Vec3( 3.0, 11.0,  3.0), Vec3()));
  scene->addCaster(Caster(torus,  Vec3( 5.0, 12.0, -4.0), Vec3()));

  // The interior doesn't case any shadows...  they work, but kinda buggy, I
  // think something to do with the fact that they it's basically an inverted
  // model.
  scene->addCaster(Caster(interior, Vec3(), Vec3(), false));
	
	// Initialise the light setup here. The first two lights are animated, they
	// just circle around a fixed path. The other lights are static.
	//               Initial Light Position           Light Color
	Light light1(Vec3( 2.0f, 6.0f,  2.0f, 1.0f), Vec3(0.2f, 0.2f, 0.2f));
	Light light2(Vec3( 0.0f, 8.0f,  0.0f, 1.0f), Vec3(0.4f, 0.4f, 0.4f));
	Light light3(Vec3( 5.0f, 6.0f,  5.0f, 1.0f), Vec3(0.2f, 0.2f, 0.6f));
	Light light4(Vec3( 5.0f, 6.0f, -5.0f, 1.0f), Vec3(0.2f, 0.4f, 0.2f));
	Light light5(Vec3(-5.0f, 6.0f, -5.0f, 1.0f), Vec3(0.5f, 0.4f, 0.1f));
	Light light6(Vec3(-5.0f, 6.0f,  5.0f, 1.0f), Vec3(0.2f, 0.1f, 0.1f));

	scene->addLight(light1);
	scene->addLight(light2);
	scene->addLight(light3);
	scene->addLight(light4);
	scene->addLight(light5);
	scene->addLight(light6);

  // Initialise global status variables.
  global.drawPointLights   = true;
  global.drawAmbientOnly   = false;
  global.drawShadows       = true;
  global.drawShadowVolumes = false;
  global.drawTextures      = true;
  global.maxVisibleLights  = 1; // Initial number of lights.
  global.animate           = true;
  global.drawSilhouettes   = false;
}


//
// Desctructor deletes all objects constructed on the heap.
//
Station::~Station()
{
  delete interior;
  delete cube;
  delete sphere;
  delete torus;

	delete cam;
	delete scene;
	delete renderer;
}


#define CHECK(x) (global.(x) ? "on\n" : "off\n" )


//
// Wrapper class for drawing a scene in OpenGL.
//
void Station::render()
{
	renderer->drawScene(*scene, *cam);
	
  /*
	// Prepare the status string for drawing.
	string fps(getFps());
	fps += "fps\n";
	fps += "Number of lights being drawn" + string(global.maxVisibleLights)
	    + "\n";
	fps += "Shadows: " . CHECK(drawShadows);
	fps += "Shadow Volumes: " . CHECK(drawShadowVolumes);
	
	renderer->drawText(fps);
  */

  char buff[16];
  sprintf(buff, "%5d FPS", static_cast<int>(getFps()));
  renderer->drawText(string(buff));
}


//
// Updates the state of the game world. In this demonstration, the update
// function below alters the rotational positions of several lights and
// Casters.
//
void Station::update(const uint& timePassed)
{
	cam->update(timePassed / 1000.0f);

  if (global.animate)
  {
    static float lightAngle1 = 0.0f;
    static float lightAngle2 = 0.0f;
    
    // Light animations.
    scene->lights[0].pos.x = 4 * sin(lightAngle1);
    scene->lights[0].pos.z = 4 * cos(lightAngle1);
    scene->lights[1].pos.x = 4 * sin(lightAngle2) + 6.0f;
    scene->lights[1].pos.y = 4 * cos(lightAngle2) + 6.0f;

    lightAngle1 += timePassed / 1000.0f;
    lightAngle2 += timePassed / 800.0f;

    // Model animations.
    scene->casters[4].rotate(Vec3(timePassed / 100.0f, 0.0f, 0.0f));
    scene->casters[4].rotate(Vec3(0.0f, 0.0f, timePassed / 50.0f));
    scene->casters[1].rotate(Vec3(timePassed / 80.0f, 0.0f, 0.0f));
    scene->casters[1].rotate(Vec3(timePassed / 30.0f, 0.0f, 0.0f));
  }
}


//
// Called on a resize event.
//
void Station::resize(const uint& newWidth, const uint& newHeight)
{
	renderer->resize(newWidth, newHeight);
}


//
// Handles key presses.
//
void Station::keyDown (const int& key)
{
  switch (key)
  {
    case SDLK_UP:
    case SDLK_w:
      cam->startMoving(Camera::FORWARD);
      break;
    case SDLK_DOWN:
    case SDLK_s:
      cam->startMoving(Camera::BACKWARD);
      break;
    case SDLK_LEFT:
    case SDLK_a:
      cam->startMoving(Camera::LEFT);
      break;
    case SDLK_RIGHT:
    case SDLK_d:
      cam->startMoving(Camera::RIGHT);
      break;

    case SDLK_MINUS:
      global.maxVisibleLights--;
      if (global.maxVisibleLights < 0)
        global.maxVisibleLights = 0;
      break;
    
    case SDLK_EQUALS:
      global.maxVisibleLights++;
      break;
    
    case SDLK_SPACE:
      global.animate = !global.animate;
      break;
    
    case SDLK_v:
      global.drawShadowVolumes = !global.drawShadowVolumes;
      break;
    
    case SDLK_c:
      global.drawShadows = !global.drawShadows;
      break;

    case SDLK_b:
      global.drawPointLights = !global.drawPointLights;
      break;
  }
}


//
// Handles key releases.
//
void Station::keyUp (const int& key)
{
  switch (key)
  {
    case SDLK_UP:
    case SDLK_w:
      cam->stopMoving(Camera::FORWARD);
      break;
    case SDLK_DOWN:
    case SDLK_s:
      cam->stopMoving(Camera::BACKWARD);
      break;
    case SDLK_LEFT:
    case SDLK_a:
      cam->stopMoving(Camera::LEFT);
      break;
    case SDLK_RIGHT:
    case SDLK_d:
      cam->stopMoving(Camera::RIGHT);
      break;
  }
}


//
// Handles moving the camera depending on the position of the mouse and whether
// the mouse is currently in a state for moving/zooming.
//
void Station::mouseMotion(const int& btn, const int& mx, const int& my)
{	
  cam->setMousePos(mx, my);
}


//
// Handles mouse button releases.
//
void Station::mouseBtnUp(const int& btn, const int& mx, const int& my)
{
	if (btn == SDL_BUTTON_LEFT)
    cam->stopRoting();
}


//
// Handles mouse button presses.
//
void Station::mouseBtnDown(const int& btn, const int& mx, const int& my)
{
	if (btn == SDL_BUTTON_LEFT)
		cam->startRoting();
}


//
// Main function.
//
int main(int argc, char *argv[])
{
  Station* game = new Station;

  game->start();

  delete game;
  return 0;
}
