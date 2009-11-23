//
// camera.h
//
// Another simple data-structure class, mostly used for passing around data
// wihtout using global variables. Plus makes things a little easier to extend
// in the future.
//


#ifndef _CAMERA_H_
#define _CAMERA_H_


#include "../math/vec3.h"
#include "../math/matrix.h"


class Camera
{

private:

	Vec3 pos;
	Vec3 rot;
	
	bool moving[4];
  bool roting;

	// Record the mouse position.
	int mousePos[2];
  int oldMousePos[2];

  float rotSpeed;
  float moveSpeed;

  Matrix worldToCam;

  bool dirtyMatrix;

public:

  enum CamDir
  {
    FORWARD  = 0,
    BACKWARD = 1,
    LEFT     = 2,
    RIGHT    = 3
  };
	
	Camera (const Vec3& pos, const Vec3& rot)
    : pos(pos), rot(rot), roting(false), rotSpeed(500.0f), moveSpeed(10.0f)
	{
		mousePos[0]    = mousePos[1]    = 0;
		oldMousePos[0] = oldMousePos[1] = 0;

    moving[0] = moving[1] = moving[2] = moving[3] = false;
	}

  const Vec3& getPosition (void) const
  { return pos; }

  const Vec3& getRotation (void) const
  { return rot; }

  const bool& isMoving (CamDir dir) const
  { return moving[dir]; }

  const bool& isRoting (void) const
  { return roting; }

  bool startMoving (CamDir dir);
  bool stopMoving (CamDir dir);

  void startRoting (void);
  void stopRoting (void);

  void setMousePos(const int& x, const int& y);

  void update (const float& timePassed);

  const Matrix& getWorldToCamMatrix (void);

  void printDetails() const;
};


#endif // _CAMERA_H_
