//
// scene.h
//
// A Scene is a data structure which holds information about a visual scene,
// including lighting, models and other misc parameters that are required.
//
// As this is only a basic data structure, I havn't been too stressed with a
// well designed interface, indeed in this instance, all members are public.
//


#ifndef _SCENE_H_
#define _SCENE_H_


#include <vector>
using std::vector;


#include "light.h"
#include "caster.h"


class Scene
{

public:

	vector<Light> lights;
	vector<Caster> casters;

	void addLight(const Light& light)
	{
		lights.push_back(light);
	}

	void addCaster (const Caster& caster)
	{
		casters.push_back(caster);
	}

  //
  // Sets all the silhouettes to dirty, meaning they'll be recalculated
  // next time they're requested.
  //
  void dirtyAllCasters()
  {
    for (int i = 0; i < casters.size(); ++i)
      casters[i].setDirtySilhouette(true);
  }
};


#endif // _SCENE_H_

