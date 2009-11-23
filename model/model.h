//
// model.h
//
// The basic model class used by the station engine. A model is a collection
// of vertices, triangles and edges which form a solid. This model only
// supports trianglular meshes at the present stage.
//

#ifndef _MODEL_H_
#define _MODEL_H_


#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl.h>
#include <vector>
#include <stdexcept>

#include "../math/vec3.h"
#include "light.h"

using std::vector;


class Model; // Forward decleration for Edge.
class Texture;


//
// This model class only supports triangular meshes, or meshes whose faces
// are entirely made of 3 sided polygons.
//
struct Face
{
  int index[3];                 // Compact vertex representation.
  int vStart;                   // First real vertex index.

  bool lightFacing;

  Vec3 normal;

  Face(void)
    : lightFacing(false)
  { }
};


//
// In this model class, an edge is able to connect 2 (and only 2) vertices,
// and should always be defining one edge of two different triangles.
//
struct Edge
{

  int v1, v2;                   // 'Real' vertex indexes.
  int vb1, vb2;                 // Vertex Buffer indexes.
  int f1, f2;                   // Face indexes.

  Model *model;                 // Pointer to the parent model.


  //
  // Mostly used constructor, invoked when building a model.
  //
  Edge (const int& v1, const int& v2, const int& f1, Model *model)
    : v1(v1), v2(v2), f1(f1), f2(-1), model(model)
  { }


  //
  // Used by reverse to create a reversed edge.
  //
  Edge (const int& v1, const int& v2, const int& f1, const int& f2,
      Model *model)
    : v1(v1), v2(v2), f1(f1), f2(f2), model(model)
  { }


  //
  // Adds the second face when found.
  //
  void addSecondFace (const int& f2)
  {
    this->f2 = f2;
  }


	//
	// Returns an edge which is the reverse of the current edge, ie the vertices
	// and faces are in the reverse order.
	//
  const Edge reverse (void)
  {
    return Edge(v2, v1, f2, f1, model);
  }


	//
	// Returns a reference to the Vector that represents one of the two vertices
	// which make up this Edge.
	//
	Vec3& getVertex (const int& which) const;
  Face& getFace (const int& which) const;

};


typedef vector<Edge> EdgeArray;


//
// The model class is specifically designed for finding shadow volumes and
// possible silhouettes of the given mesh.
//
class Model
{

private:

  GLuint vBuff, nBuff, tBuff, eBuff;
  bool usingVertexBuffers;

public:

  // ------------------------------------------------------------------------
  // Data Members
  // ------------------------------------------------------------------------

  // Basic Geometric details of the mesh.
  vector<Vec3> vertArray;
  vector<Vec3> textArray;
  vector<Vec3> normArray;

  // These sets are mostly used for shadow volume determination.
  vector<Face>  faceArray;
  EdgeArray edgeArray;

  // A more compact version of the vertex positional array.
  vector<Vec3> realVerts;

  // Just for efficientcy.
  bool hasNormals;
  bool hasTexCoords;

  Texture *tex;

  // ------------------------------------------------------------------------
  // Interface
  // ------------------------------------------------------------------------

  Model(void)
    : usingVertexBuffers(false), hasNormals(false), hasTexCoords(false),
    tex(NULL)
  { }

  ~Model(void);

  // Accessors for the different arrays.
  void addVertex(const Vec3& v)
  { vertArray.push_back(v); }

  void addNormal(const Vec3& n)
  { normArray.push_back(n); }

  void addTexCoord(const Vec3& t)
  { textArray.push_back(t); }

  // Provides an interface for getting information about the model for OpenGL
  // rendering.
  const GLfloat *getVertexPointer(void) const
  { return (GLfloat *) &(vertArray[0]); }

  const GLfloat *getNormalPointer(void) const
  { return (GLfloat *) &(normArray[0]); }

  const GLfloat *getTexCoordPointer(void) const
  { return (GLfloat *) &(textArray[0]); }

  const int faceCount( void ) const
  { return faceArray.size(); }

  const int getVertexCount(void) const
  { return vertArray.size(); }

  const int getRealVertexCount(void) const
  { return realVerts.size(); }

  const Vec3& getRealVertex(const int& i) const
  { return realVerts[i]; }

  EdgeArray& getEdgeArray()
  { return edgeArray; }

  void calcFaceNormals(void);

  // ------------------------------------------------------------------------
  // Drawing interface.
  // ------------------------------------------------------------------------
  void initVertexBuffers (void);

  void drawVertexBuffers (void);

  void bindExtrudeBuffer (void);

  // Used for drawing shadow volumes.
  void drawIndexQuad (const int& a, const int& b, const int& c, const int& d);
  void drawIndexTri  (const int& a, const int& b, const int& c);
};


#endif // _MODEL_H_

