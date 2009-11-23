//
// camera.cpp
//
// Helper functions for camera motions.
//


#include "camera.h"
#include <cstdio>




//
// Sets the camera to move in a specified direction, returns the old value
// for moving in that direction.
//
bool Camera::startMoving (CamDir dir)
{
  if (moving[dir]) return true;

  moving[dir] = true;
  return false;
}


//
// Sets the camera to stop for a specified direction, returns the old value
// for moving in that direction.
//
bool Camera::stopMoving (CamDir dir)
{
  if (!moving[dir]) return false;

  moving[dir] = false;
  return true;
}


//
// Start the camera rotations.
//
void Camera::startRoting()
{
  roting = true;
}


//
// Stop the camera rotations
//
void Camera::stopRoting()
{
  roting = false;
}


//
// Swaps the current mouse positions and updates the new coordinates.
//
void Camera::setMousePos(const int& x, const int& y)
{
  if (roting)
  {
    rot.x += (y - mousePos[1]) * 0.5;
    rot.y += (x - mousePos[0]) * 0.5;

  }

  mousePos[0] = x;
  mousePos[1] = y;
}


//
// Updates the position of the camera depending on the time passed and
// the motion setting the camera has.
//
void Camera::update (const float& time)
{
  Vec3 move;
  bool moved;
  
  // Directional movements. Converts between Euler angles and positional
  // vectors. Sin and Cos functions here are only ever classed a few times
  // per frame and don't incur much penalty.
  if (moving[FORWARD])
  {
    moved = true;
    float cosy = cos(RAD(rot.x));
    
    move.x = -cosy * sin(RAD(rot.y));
    move.z =  cosy * cos(RAD(rot.y));
    move.y =  sin(RAD(rot.x));
    
    move.unitize();
    move *= time * moveSpeed;
    
    pos += move;
  }
  if (moving[BACKWARD])
  {
    moved = true;
    float cosy = cos(RAD(rot.x));
    
    move.x = -cosy * sin(RAD(rot.y));
    move.z =  cosy * cos(RAD(rot.y));
    move.y =  sin(RAD(rot.x));
    
    move.unitize();
    move *= time * -moveSpeed;
    
    pos += move;
  }
  if (moving[RIGHT])
  {
    moved = true;
    move.x = cos(RAD(rot.y));
    move.z = sin(RAD(rot.y));
    move.y = 0.0f;
    
    move.unitize();
    move *= time * -moveSpeed;
    
    pos += move;
  }
  if (moving[LEFT])
  {
    moved = true;
    move.x = cos(RAD(rot.y));
    move.z = sin(RAD(rot.y));
    move.y = 0.0f;
    
    move.unitize();
    move *= time * moveSpeed;
    
    pos += move;
  }

  // Update the World to Cam matrix.
  if (moved) dirtyMatrix = true;
}


//
// Reuturns a matrix which translates world coordinates into eye-space or
// camera coordinates.
//
const Matrix& Camera::getWorldToCamMatrix ()
{
  worldToCam.fromAngles(rot);

  Matrix trans;
  trans.identify();
  trans.setTranslation(pos);

  worldToCam = worldToCam * trans;

  return worldToCam;
}


//
// Just prints out some details about the camera position. Mostly only used
// for initial debugging purposes.
//
void Camera::printDetails () const
{
  printf("Position is (%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z);
  printf("Rotation is (%.2f, %.2f, %.2f)\n", rot.x, rot.y, rot.z);
}

