/**
 * basegame.cpp
 */


#include <iostream>

#include "basegame.h"


/**
 * Constructor. This constructor has default parameters, so it also acts as
 * the default constructor.
 * NOTE: Generally, it's the child classes rensponsibility to call the
 * resize method from its constructor.
 */
BaseGame::BaseGame(const uint& width, const uint& height, const uint& flags)
	: screen(NULL), lastTime(0), thisTime(0), winWidth(width),
	winHeight(height), winTitle(""), running(true), fpsTicks(0),
	fpsCounter(0), fpsCurrent(0), flags(flags)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		cout << "Unable to init SDL: " << SDL_GetError() << endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	screen = SDL_SetVideoMode(width, height, 32, flags);

	if(!screen)
	{
		cout << "Unable to init video: " << SDL_GetError() << endl;
		exit(1);
	}
}


/**
 * Destructor.
 */
BaseGame::~BaseGame()
{
	SDL_Quit();
}


/**
 * Main BaseGame Loop.
 */
void BaseGame::start()
{
	while (running)
	{
		pEvents();
		pUpdate();
		pRender();
	}
}


/**
 * Updates the current game state.
 */
void BaseGame::pUpdate()
{
	thisTime = SDL_GetTicks();
	long timePassed = thisTime - lastTime;

	update(timePassed);

	fpsTicks += timePassed;
	lastTime  = thisTime;
}


/**
 * Renders a frame of the current game.
 */
void BaseGame::pRender()
{
	// Calculate the current FPS.
	++fpsCounter;
	if (fpsTicks > 1000)
	{
		fpsCurrent = fpsCounter / (fpsTicks / 1000.0);
		fpsCounter = 0;
		fpsTicks   = 0;
	}

	render();

	SDL_GL_SwapBuffers();
}


/**
 * Handles application input.
 */
void BaseGame::pEvents()
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				running = false;
				break;
			}
			keyDown(event.key.keysym.sym);
			break;

			case SDL_KEYUP:
			keyUp(event.key.keysym.sym);
			break;

			case SDL_QUIT:
			running = false;
			break;

			case SDL_VIDEORESIZE:
			screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 32, flags);
			resize(event.resize.w, event.resize.h);
			break;

			case SDL_MOUSEMOTION:
			mouseMotion(event.button.button, event.motion.x, event.motion.y);
			break;

			case SDL_MOUSEBUTTONUP:
			mouseBtnUp(event.button.button, event.motion.x, event.motion.y);
			break;

			case SDL_MOUSEBUTTONDOWN:
			mouseBtnDown(event.button.button, event.motion.x, event.motion.y);
			break;
		}
	}
}


/**
 * Set the current window title.
 */
void BaseGame::setTitle(const string& title)
{
	winTitle = title;
	SDL_WM_SetCaption(winTitle.c_str(), 0);
}


/**
 * Get the window name.
 */
const string& BaseGame::getTitle() const
{
	return winTitle;
}


/**
 * Return the current screen surface.
 */
SDL_Surface *BaseGame::getScreen() const
{
	return screen;
}


/**
 * Get the current fps.
 */
const float& BaseGame::getFps() const
{
	return fpsCurrent;
}
