//
// light.h
//

#ifndef _LIGHT_H_
#define _LIGHT_H_


#include "../math/vec3.h"

//
//
//
class Light 
{

public:

  Vec3 pos;
  Vec3 color;

  Light(const Vec3& pos) : pos(pos), color(1.0, 1.0, 1.0)
  { }

  Light(const Vec3& pos, const Vec3& color) : pos(pos), color(color)
  { }

  const Vec3& getPosition (void) const
  { return pos; }
};


#endif // _LIGHT_H_

