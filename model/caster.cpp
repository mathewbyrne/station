//
// caster.cpp
//
// Implementation of the Caster function.
//


#include "caster.h"


// Used to fix silhouette calculations by providing a threshold for float
// values to equal 0.
const float ZERO_THRESHOLD = 0.0001f;


//
// Calculates a matrix for a Caster if the current Matrix requires updating.
// The dirtyMatrix member tracks the state of this.
//
const Matrix& Caster::getLocalToWorldMatrix (void)
{
  if (dirtyMatrix)
  {
    localToWorld.fromAngles(rot);
    Matrix trans = getTranslationMatrix(pos);
    localToWorld = trans * localToWorld;

    dirtyMatrix = false;
  }

  return localToWorld;
}


//
// For each face in the model, finds whether the face is facing towards or away
// from a light source. This function (like getSilhouette) assumes that the
// light position is in local space not global.
//
void Caster::findLightFacing(const Vec3& lightPos)
{
  for(vector<Face>::iterator it = model->faceArray.begin();
      it != model->faceArray.end(); ++it)
  {
    it->lightFacing = dot(it->normal,
        lightPos.w * model->vertArray[it->vStart] - lightPos) > ZERO_THRESHOLD;
  }
}


//
// Given a light source position, this function calculates a list of edges
// which form the silhouette boundary between faces which faces towards the
// light source, and faces that face away from the source.
//
EdgeArray& Caster::getSilhouette(const Vec3& lightPos)
{
  if (dirtySilhouette)
  {
  	silhouette.clear();

    findLightFacing(lightPos);

    for(EdgeArray::iterator edge = model->edgeArray.begin();
        edge != model->edgeArray.end(); ++edge)
    {
	  	bool lf1 = (*edge).getFace(0).lightFacing;
		  bool lf2 = (*edge).getFace(1).lightFacing;

      if ((lf1 && !lf2) || (!lf1 && lf2))
      {
        // Make sure that the edge is oriented properly.
        if (lf1)
          silhouette.push_back(*edge);
        else
          silhouette.push_back((*edge).reverse());
      }
    }

    dirtySilhouette = false;
  }

  return silhouette;
}


//
// Moves a Caster by a certain amount.
//
void Caster::translate (const Vec3& pos)
{
  this->pos += pos;
  dirtyMatrix = true;
}


//
// Moves a Caster to a set position.
//
void Caster::setTranslation (const Vec3& pos)
{
  this->pos = pos;
  dirtyMatrix = true;
}


//
// Rotates a Caster by a set amount.
//
void Caster::rotate (const Vec3& rotAdd)
{
  this->rot += rotAdd;
  if (rot.x > 360.0f) rot.x -= 360.0f;
  if (rot.y > 360.0f) rot.y -= 360.0f;
  if (rot.z > 360.0f) rot.z -= 360.0f;
  dirtyMatrix = true;
}


//
// Rotates a Caster to a set angle.
//
void Caster::setRotation (const Vec3& rot)
{
  this->rot = rot;
  dirtyMatrix = true;
}

