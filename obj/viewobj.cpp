//
// This code was adapted from the Obj loading toolkit provided by Alex.
//
// Object viewing test appliction. Loads in an obj file in it's simplest manner
// and displays a lit model. It does some other stuff, and initially this was
// used to fix a lot of the more obvious bugs in the code, now not a part of
// the final executable...
//
//       ...and not very well documented, but read on!
//

#include <iostream>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include "obj.h"

#include "../math/matrix.h"

using namespace std;


static ObjModel *model;

static Light *light;

static float cam_z;
static float rotX;
static float rotY;
static int   oldX;
static int   oldY;


//
// Returns the current Model-View matrix from OpenGL in a Matrix class.
//
Matrix getWorldMatrix()
{
	float mvMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mvMat);
	return Matrix(mvMat);
}


void init()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);

  float pos[4] = {1, 1, 1, 0};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  light = new Light(Vec3(1, 1, 1), LT_DIRECTIONAL);

  Vec3 min, max;
  model->findBoundingBox(min, max);
  printf("Bounding box (%f, %f, %f)-(%f, %f, %f)\n",
          min.x, min.y, min.z, max.x, max.y, max.z);
  cam_z = max.z * 3;
}


void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0, 0, -cam_z);
  glRotatef(rotX, 1, 0, 0);
  glRotatef(rotY, 0, 1, 0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer  (3, GL_FLOAT, sizeof(Vec3), model->getVertexPointer());
  glNormalPointer  (   GL_FLOAT, sizeof(Vec3), model->getNormalPointer());
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vec3), model->getTexCoordPointer());

  //glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());

// ----------------------------------------------------------------------------
// Silhouette Calculations.
// ----------------------------------------------------------------------------
	Matrix world2Local = getWorldMatrix();
	world2Local.invert();
  Vec3 lightPos = light->getPosition();
  world2Local.transform(lightPos);
  EdgeArray& sil = model->getSilhouette(lightPos);
// ----------------------------------------------------------------------------

  glDisable(GL_LIGHTING);
  for(EdgeArray::const_iterator edge = sil.begin();
      edge != sil.end(); ++edge)
  {
    Vec3& a = edge->getVertex(0);
    Vec3& b = edge->getVertex(1);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
      glVertex3f(a.x, a.y, a.z);
      glVertex3f(b.x, b.y, b.z);
    glEnd();
  }
  glEnable(GL_LIGHTING);

  int er = glGetError();
  if (er) printf("%s\n", gluErrorString(er));

  glutSwapBuffers();
}


void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, w / (float) h, 1.0, 20000.0);
  glMatrixMode(GL_MODELVIEW);
}


void mouse(int button, int state, int x, int y)
{
  oldX = x;
  oldY = y;
  if (button == 3)
    cam_z--;
  else if (button == 4)
    cam_z++;
}


void motion(int x, int y)
{
  rotY += x - oldX;
  rotX += y - oldY;
  oldX = x;
  oldY = y;
}


void idle()
{
  glutPostRedisplay();
}


int main(int argc, char **argv)
{
  if (argc != 2)
  {
    cerr << "Incorrect number of argumets.";
    exit(1);
  }

  glutInit(&argc, argv);
  string filename(argv[1]);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow(filename.c_str());

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);

  model = new ObjModel(filename);

  init();

  glutMainLoop();
  return EXIT_SUCCESS;
}
