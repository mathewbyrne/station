//
// caster.h
//
// A Caster is an instance of a model that can cast light and has a position
// that can be represented as a Matrix. The main reason for this class is
// to save on the memory wasted by creating two Model classes. It also helps
// to abstract some of the shadow code out of the Model class.
//


#ifndef _CASTER_H_
#define _CASTER_H_


#include "model.h"
#include "../math/vec3.h"
#include "../math/matrix.h"


class Caster
{
  
private:
  
  Model *model;

  Vec3 pos;
  Vec3 rot;

  Matrix localToWorld;
  bool dirtyMatrix;

  bool caster;

  void findLightFacing(const Vec3& lightPos);
  EdgeArray silhouette;
  bool dirtySilhouette;

public:

  Caster (Model *model, const Vec3& pos, const Vec3& rot)
    : model(model), pos(pos), rot(rot), dirtyMatrix(true),
      caster(true), dirtySilhouette(true)
  { }

  Caster (Model *model, const Vec3& pos, const Vec3& rot, const bool& caster)
    : model(model), pos(pos), rot(rot), dirtyMatrix(true),
      caster(caster), dirtySilhouette(true)
  { }

  // Accessors.
  Model *getModel (void) const
  { return model; }

  const bool& isCaster (void) const
  { return caster; }

  const Vec3& getTranslation (void) const
  { return pos; }

  const Vec3& getRotation (void) const
  { return rot; }

  const Matrix& getLocalToWorldMatrix (void);

  EdgeArray& getSilhouette (const Vec3& lightPos);

  // Mutators.
  void translate (const Vec3& pos);
  void setTranslation (const Vec3& pos);

  void rotate (const Vec3& rot);
  void setRotation (const Vec3& rot);

  void setCaster (const bool& caster)
  { this->caster = caster; }

  void setDirtySilhouette (const bool& dirty)
  { dirtySilhouette = dirty; }
};


#endif // _CASTER_H_

