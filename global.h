//
// global.h
//


#ifndef _GLOBAL_H_
#define _GLOBAL_H_


//
// Global struct for holding control or state variables that can be read by
// everything.
//
struct Global
{
  bool drawPointLights;
  bool drawShadows;
  bool drawShadowVolumes;
  bool drawSilhouettes;
  bool drawTextures;

  bool drawAmbientOnly;
  
  bool animate;

  int maxVisibleLights;

  int winWidth;
  int winHeight;

};


#endif

