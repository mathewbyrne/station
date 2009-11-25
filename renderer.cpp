//
// renderer.cpp
//
// Contains the implementation of most of the drawing code for the assignment.
// All drawing is done in OpenGL.
//


#include "renderer.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "model/model.h"
#include "material/shader.h"
#include "material/texture.h"
#include "font/font.h"


Global global;


//
// This functions uses changed a little over time. In it's present form, this
// function prints a side by side comparison of the current Modelview Matrix
// from OpenGL, and the Matrix I generated personally. This was just to make
// sure I was generating valid matrixes which did what I wanted.
//
void Renderer::getModelviewMatrix(Camera& cam)
{
	float values[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, values);
  const Matrix& mat = cam.getWorldToCamMatrix();

  printf("---------------------------------------------------------------\n");
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
      printf("%0.3f ", values[i * 4 + j]);

    printf("\t");

    for (int j = 0; j < 4; j++)
      printf("%0.3f ", mat.values[i * 4 + j]);

    printf("\n");
  }
}


//
// Prints out a formatted matrix to stdout.
//
void Renderer::printMatrix(const Matrix& m)
{
  printf("\n");
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
      printf("%.2f ", m.values[i * 4 + j]);
    printf("\n");
  }
}


//
// This is a static function used to A) turn on the stencil buffer and B) to
// set it's tests based on different front and back tests.
//
void Renderer::setStencilOp(const GLenum& frontDepthFail,
    const GLenum& frontDepthPass, const GLenum& backDepthFail,
    const GLenum& backDepthPass)
{
  // Stencil operations for front and back faces differ.
  glStencilOpSeparate(GL_FRONT, GL_KEEP, frontDepthFail, frontDepthPass);
  glStencilOpSeparate(GL_BACK, GL_KEEP, backDepthFail, backDepthPass);
}


//
// Performs all the initial setting of the OpenGL state machine.
//
Renderer::Renderer (void)
{
  resize(800, 600);

	glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set the widths of lines and points (mostly for silhouette displays)
  glPointSize(16.0);
  glLineWidth(8.0);

  // This extension clamps anything drawn past the far plane to be clamped to
  // the far plane. This is needed for z-fail shadows when drawing a dark
  // cap.
  // TODO: Create a custom perspective matrix so that this extension is
  // not required.
  glEnable(GL_DEPTH_CLAMP_NV);

  extrudeShader = new ShaderProgram("extrude", "data/shaders/extrude.vert",
      "");
  font = new Font("data/vera.ttf", 32);
}


//
// Renderer clean-up
//
Renderer::~Renderer (void)
{
  delete extrudeShader;
  delete font;
}


//
// Main drawing function. Renders a completed scene to the screen.
//
void Renderer::drawScene(Scene& scene, Camera& camera)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
      GL_STENCIL_BUFFER_BIT);

  // Load the viewing translations.
  resize(global.winWidth, global.winHeight);
  glLoadMatrix(camera.getWorldToCamMatrix());

  // Unlit scene + Depth Buffer info.
  ambientPass(scene, camera);

  // Only enter this loop if ambient only is not enabled.
  if (!global.drawAmbientOnly)
  {
    // The rest of the rendering is done on a 'per-light' basis, shadows are
    // determined for each light and the scene is additively illuminated.
  	for (int i = 0; i < scene.lights.size(); ++i)
  	{
  	  if (i >= global.maxVisibleLights) break;

  	  Light& light = scene.lights[i];
  	  
      // Setup the light for drawing and draw it.
      setupLight(light);
      if (global.drawPointLights)
        drawLight(light);

      // Determine shadows and light the scene.
      if (global.drawShadows)
      {
        determineShadows(scene.casters, light, camera);
      }
      
      // Iluminate the scene fro this light.
      illuminationPass(scene, camera);

	glClear(GL_STENCIL_BUFFER_BIT);
  	}
  	
    scene.dirtyAllCasters();
	}

	// Check for OpenGL errors.
	int er = glGetError();
	if (er) printf("%s\n", gluErrorString(er));
}


//
// Called whenever a resize occurs. Basically justs resets the OpenGL
// viewport settings to suit the new window size.
//
void Renderer::resize (const uint& newWidth, const uint& newHeight) const
{
	glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat) newWidth / (GLfloat) newHeight, 0.1, 128.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Set the width and height at a global level.
	global.winWidth = newWidth;
	global.winHeight = newHeight;
}


//
// Sets up a light for use with OpenGL.
//
void Renderer::setupLight (const Light& light)
{
  glLightfv(GL_LIGHT0, GL_POSITION, light.pos.v);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light.color.v);
}


//
// glVertex method for using the Vec3(4) class in this project.
//
void Renderer::glVertex (const Vec3& v)
{
  glVertex4fv(v.v);
}


//
// glLoadMatrix method for using the Matrix class in this assignment.
//
void Renderer::glLoadMatrix (const Matrix& m)
{
  glLoadMatrixf(m.values);
}


//
// glMultMatrix method for using a Matrix object.
//
void Renderer::glMultMatrix (const Matrix& m)
{
  glMultMatrixf(m.values);
}


//
// Renders a point for a light.
//
void Renderer::drawLight (const Light& light)
{
  // Directional lights cannot be drawn.
  if (light.pos.w == 0) return;
    
  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);

  glColor4fv(light.color.v);
  glBegin(GL_POINTS);
  glVertex(light.getPosition());
  glEnd();

  glPopAttrib();
}


//
// Draws the edges of a silhouette, mostly just used for debugging pyrposes
// but does give an informative view of what the silhouette looks like.
//
void Renderer::drawSilhouette(EdgeArray& sil) const
{
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);

	// Loop through each edge in the silhouette array, get a Vertex
  for(EdgeArray::const_iterator edge = sil.begin();
      edge != sil.end(); ++edge)
  {
    glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);
      glVertex(edge->getVertex(0));
      glColor3f(0.0, 1.0, 0.0);
      glVertex(edge->getVertex(1));
    glEnd();
  }

	glPopAttrib();
}


//
// Ambient pass for the rendering algorithm. This is done only once per frame,
// and renders the entire scene without lighting speficic detail. Even if there
// is no ambient light, the Dpeth Buffer information is still needed for later
// shadow determination.
//
void Renderer::ambientPass (const Scene& scene, Camera& camera)
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_DEPTH_TEST);              // Enable Depth testing.
	glDisable(GL_LIGHT0);                 // Disable the main light.
	glEnable(GL_CULL_FACE);               // Enable back-face culling.
	glDepthMask(1);                       // Enable depth buffer drawing.
	glColorMask(1, 1, 1, 1);              // Enable frame buffer drawing.

  // Loop through all casters in the scene and draw them without any
  // lighting.
	for (int i = 0; i < scene.casters.size(); ++i)
	{
    Caster caster = scene.casters[i];

    glPushMatrix();
    glMultMatrix(caster.getLocalToWorldMatrix());

    // Draw the model from a Vertex Buffer.
    caster.getModel()->drawVertexBuffers();

    glPopMatrix();
	}

	glPopAttrib();
}


//
// The guts of the shadow determination algorithm.
//
void Renderer::determineShadows (vector<Caster> casters, const Light& light,
    Camera& camera)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDepthMask(0);                       // Disable depth buffer changes.
  glDisable(GL_CULL_FACE);              // Enable drawing of backfaces.

  glEnable(GL_STENCIL_TEST);            // Enable the stencil buffer.
  glStencilFunc(GL_ALWAYS, 0, ~0);      // Always pass the stencil test.
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // If we draw shadow volumes then we need to specify a color, otherwise
  // we disable drawing into the frame buffer.
  if (global.drawShadowVolumes)
  {
    glColor4f(1.0, 0.0, 0.0, 0.2);
  }
  else
  {
    glColorMask(0, 0, 0, 0);
  }

  
  for (vector<Caster>::iterator caster = casters.begin();
      caster != casters.end(); ++caster)
  {
    if (!caster->isCaster())
      continue;
    
    // Get the matricies required to draw the caster, and load a matrix which
    // places the caster into Camera space.
    glPushMatrix();
    const Matrix& localToWorld = caster->getLocalToWorldMatrix();
    glMultMatrix(localToWorld);

    // Transform the light position into object (or local) space by inverting
    // the localToWorld matrix and transforming the light position.
    Vec3 lightPosLocal = light.getPosition();
    invertMatrix(localToWorld).transform(lightPosLocal);


    caster->getModel()->bindExtrudeBuffer();

    // Setup the shader program for use.
    extrudeShader->useProgram();
    glUniform4f(glGetUniformLocation(extrudeShader->getId(), "lightPos"),
        lightPosLocal.x, lightPosLocal.y, lightPosLocal.z, lightPosLocal.w);

    // TODO: Add z-Fail testing here.
    // z-Pass algorithm.
    //setStencilOp(GL_KEEP, GL_INCR_WRAP, GL_KEEP, GL_DECR_WRAP);
    //drawVolumeSides(lightPosLocal, *caster);
    
    // z-fail method (Carmacks Reverse). Works for nearly all situations but
    // isn't as efficient as the z-pass method above as it draws both the
    // front and back caps whereas the other methods doesn't require this.
    if (1)
    {
      setStencilOp(GL_DECR_WRAP, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
      drawVolumeSides(lightPosLocal, *caster);
      drawDarkCap(lightPosLocal, *caster);
      drawLightCap(lightPosLocal, *caster);
    }
    
    /*

    // This is an alternative method for drawing the shadow volumes which
    // works but takes two passes and isn't as efficient. This is the
    // z-pass method too.
       
    glEnable(GL_CULL_FACE);
    
    glCullFace(GL_BACK);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    drawVolumeSides(lightPosLocal, *caster);
    
    glCullFace(GL_FRONT);
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
    drawVolumeSides(lightPosLocal, *caster);

    */
    
    extrudeShader->disableProgram();

    glPopMatrix();
  }

  glPopAttrib();
}


//
// This function does the actual drawing of the shadow volume sides.
//
void Renderer::drawVolumeSides (const Vec3& lightPos, Caster& caster)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);

  Model *model = caster.getModel();
  EdgeArray& sil = caster.getSilhouette(lightPos);


  int offset = model->getRealVertexCount();

  // Draw the shadow volume sides.
  for (vector<Edge>::const_iterator edge = sil.begin();
      edge != sil.end(); ++edge)
  {
    // For point lights.
    if (lightPos.w > 0)
    {
      model->drawIndexQuad(edge->v1, edge->v2,
          edge->v2 + offset, edge->v1 + offset);
      
      /*
      // This is the non-VBO Method of Volume drawing.
       
      Vec3& v1 = edge->getVertex(0);
      Vec3& v2 = edge->getVertex(1);
      Vec3  i1(edge->getVertex(0), 0.0f);
      Vec3  i2(edge->getVertex(1), 0.0f);

      glBegin(GL_QUADS);

      glVertex(v1);
      glVertex(v2);
      glVertex(i2);
      glVertex(i1);

      glEnd();
      */
    }
    // For directional lights.
    else
    {
      model->drawIndexTri(edge->v1, edge->v2,
          offset);
    }
  }

  glPopAttrib();
}


//
// Draws the Dark cap (cap at infinity) of a shadow volume.
//
void Renderer::drawDarkCap (const Vec3& lightPos, Caster& caster)
{
  // Directional lights come to a point.
  if (lightPos.w == 0.0f)
    return;
  
  EdgeArray& sil = caster.getSilhouette(lightPos);
  Model *model = caster.getModel();
  int offset = model->getRealVertexCount();

  for (EdgeArray::iterator edge = sil.begin();
      edge != sil.end(); ++edge)
  {
    caster.getModel()->drawIndexTri(offset, edge->v1 + offset,
        edge->v2 + offset);
  }
}


//
// Draws the light cap (cap at the front) of a shadow volume. Simply loops
// through all the faces in a model and draws those which are light facing,
// ie, the light cap!
//
void Renderer::drawLightCap (const Vec3& lightPos, Caster& caster)
{
  Model *model = caster.getModel();

  glDepthFunc(GL_NEVER);
  for (vector<Face>::iterator face = model->faceArray.begin();
      face != model->faceArray.end(); ++face)
  {
    if (face->lightFacing)
      model->drawIndexTri(face->index[0], face->index[1], face->index[2]);
  }
  glDepthFunc(GL_LESS);
}


//
// The final illumination pass for any single light. This pass sets the blend
// function to GL_ONE GL_ONE so that fragments are essentially added together
// with the fragments from the ambient pass. The stencil function is set to
// pass when a stencil fragment equals 0.
//
void Renderer::illuminationPass(const Scene& scene, Camera& camera)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glEnable(GL_STENCIL_TEST);
  glStencilMask(0);
  glStencilFunc(GL_EQUAL, 0, ~0);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glDepthMask(0);
  glDepthFunc(GL_EQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glColorMask(1, 1, 1, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);                // Additive blending.
  glEnable(GL_LIGHT0);                        // The required light.

  // Loop through all casters in the scene and draw them.
	for (int i = 0; i < scene.casters.size(); ++i)
	{
    Caster caster = scene.casters[i];

    glPushMatrix();
    glMultMatrix(caster.getLocalToWorldMatrix());

    // Draw the model from a Vertex Buffer.
    // caster.bindTextures();
    caster.getModel()->drawVertexBuffers();

    glPopMatrix();
	}

  glPopAttrib();
}


//
// This function draws text onto the screen.
//
void Renderer::drawText (const string& text)
{
  font->setFontScreen(global.winWidth, global.winHeight);
  glColor4f(1.0, 1.0, 1.0, 0.5);
  font->printStrLoc(4, global.winHeight - 36, text.c_str());
}
