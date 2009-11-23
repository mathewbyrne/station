//
// renderer.h
//
// Interface for the rendering system that the assignment uses to produce
// images through OpenGL. The level of abstraction isn't complete or well
// designed, but for the purposes of this assignment, they work.
//

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "global.h"
#include "ltypes.h"
#include <string>

using std::string;

// TODO: Fix the include situation.
#include "model/model.h"
#include "math/matrix.h"
#include "model/camera.h"
#include "model/scene.h"


// Global global instance in renderer.cpp :)
extern Global global;


class ShaderProgram;
class Font;


class Renderer
{

private:

  static void setStencilOp(const GLenum& frontDepthFail,
    const GLenum& frontDepthPass, const GLenum& backDepthFail,
    const GLenum& backDepthPass);

  void setupLight (const Light& light);
  static void drawLight (const Light& light);
  void ambientPass (const Scene& scene, Camera& camera);
  void determineShadows (vector<Caster> casters, const Light& light,
      Camera& camera);
  void illuminationPass (const Scene& scene, Camera& camera);

  void drawSilhouette (EdgeArray& sil) const;
  void drawVolumeSides (const Vec3& lightPos, Caster& caster);
  void drawDarkCap (const Vec3& lightPos, Caster& caster);
  void drawLightCap (const Vec3& lightPos, Caster& caster);

  // Shader Program for extrudeing vertices.
  ShaderProgram *extrudeShader;
  
  // Font object for rendering text to the screen.
  Font *font;
	
public:

  static void glVertex (const Vec3& v);
  static void glLoadMatrix (const Matrix& m);
  static void glMultMatrix (const Matrix& m);
  
	static void getModelviewMatrix (Camera& cam);
  static void printMatrix (const Matrix& m);
	
	Renderer (void);
  ~Renderer (void);
	
	void drawScene (Scene& scene, Camera& cam);
	void drawText (const string& text);
	void resize (const uint& width, const uint& height) const;

};

#endif // _RENDERER_H_

