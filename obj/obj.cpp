//
// ObjModel implementation.
//

#include "obj.h"
#include "../material/texture.h"

#include <string>
#include <cstdio>
#include <cfloat>
#include <stdexcept>


#ifndef MIN
#  define MIN(a,b) (a > b ? b : a)
#endif
#ifndef MAX
#  define MAX(a,b) (a > b ? a : b)
#endif


int yyparse(ObjModel *objFile);
extern FILE *yyin;
extern int yylineno;


// --------------------------------------------------------------------------
// NamedObject implementation - TODO: Convert this to something useful.
// --------------------------------------------------------------------------
/*
NamedObject::NamedObject( const string& name, const int& index )
  : name( name ), index( index ), next( NULL )
{ }
*/


// --------------------------------------------------------------------------
// ObjModel implementation - An ObjModel attempts to load its data at
// creation. If no filename is specified, the data can later be loaded with
// the loadFile() function.
// --------------------------------------------------------------------------


ObjModel::ObjModel(const string& filename)
  : filename(filename), faceVertNo(0)
{
  if(filename != "")
    loadFile(filename);
}


ObjModel::~ObjModel()
{ }


//
// The parsing of the OBJ file is started with this function.
//
int ObjModel::loadFile(const string& filename)
{
  ObjLoadData *ld = new ObjLoadData();
  
  yyin = fopen(filename.c_str(), "rt");
  int result = yyparse(this, ld);

  postProcessModel(*ld);

  delete ld;

  return result;
}


//
// Sets the values of the two referenced vectors to the min and max corners
// of an axis aligned BoundingBox.
//
void ObjModel::findBoundingBox(Vec3 &min, Vec3 &max) const
{
  for(int i = 0; i < 3; i++)
  {
    min.x = min.y = min.z = FLT_MAX;
    max.x = max.y = max.z = -FLT_MAX;
  }

  for(vector<Vec3>::const_iterator it = vertArray.begin();
      it != vertArray.end(); ++it)
  {
    min.x = MIN(min.x, it->x);
    min.y = MIN(min.y, it->y);
    min.z = MIN(min.z, it->z);
    max.x = MAX(max.x, it->x);
    max.y = MAX(max.y, it->y);
    max.z = MAX(max.z, it->z);
  }
}


//
// Adds a new face to the Triangle vector and increments the current counter.
//
void ObjModel::beginFace(void)
{
  Face f;
  f.vStart = vertArray.size();

  faceArray.push_back(f);
  faceVertNo = 0;
}


//
// Adds a new vertex to the current face. 
//
void ObjModel::addFaceVertex(const int& vIndex, const int& tIndex,
      const int& nIndex, const ObjLoadData& ld)
{
  int fIndex = faceArray.size();

  faceArray[fIndex - 1].index[faceVertNo] = vIndex - 1;

  if (vIndex < 0)
    addVertex(ld.vertices[ld.vertices.size() + vIndex]);
  else if(vIndex > 0)
    addVertex(ld.vertices[vIndex - 1]);

  if (tIndex < 0)
    addTexCoord(ld.texCoords[ld.texCoords.size() + tIndex]);
  else if(tIndex > 0)
    addTexCoord(ld.texCoords[tIndex - 1]);

  if (nIndex < 0)
    addNormal(ld.normals[ld.normals.size() + nIndex]);
  else if(nIndex > 0)
    addNormal(ld.normals[nIndex - 1]);

  faceVertNo++;
}


//
// Processes the model after all vertex and face data has been loaded.
// Calcualtes the edges in a uniform orientation and solves all the
// face normals.
//
void ObjModel::postProcessModel(ObjLoadData& ld)
{
  int vCount = ld.vertices.size();
  int edgeCount = 0;
  realVerts = ld.vertices;

  hasNormals   = (normArray.size() > 0);
  hasTexCoords = (textArray.size() > 0);

  // Allocate edge memory and clear the array of indexes.
  int *edgeRef = new int[SQ(vCount)];
  for (int i = 0; i < SQ(vCount); i++)
    edgeRef[i] = -1;

  for (int it = 0; it < faceArray.size(); it++)
  {
    Face& face = faceArray[it];

    // ----------------------------------------------------------------------
    // Edge Detection - Phase 1
    //
    // Loop through all triangles. The vertices are already ordered properly
    // for OpenGL drawing. So what we're going to do is add a new edge if
    // the first index is less than the second one ONLY. This will ensure
    // that each edge is only added once.
    // ----------------------------------------------------------------------
    for(int i = 0; i < 3; i++)
    {
      int ni = (i + 1) % 3;

      int i1 = face.index[i];
      int i2 = face.index[ni];
      
      if (i1 < i2)
      {
        Edge e(i1, i2, it, this);
        edgeArray.push_back(e);
        edgeRef[i1 * vCount + i2] = edgeArray.size() - 1;
      }
    }
  }

  for (int it = 0; it < faceArray.size(); it++)
  {
    Face& face = faceArray[it];

    // ----------------------------------------------------------------------
    // Edge Detection - Phase 2
    //
    // Now that we have an edge for every vertex pair, all we need to do is
    // find the opposite; every edge pair which the first index is larger
    // than the second, and fill in the missing face pointer.
    // ----------------------------------------------------------------------
    for(int i = 0; i < 3; i++)
    {
      int ni = (i + 1) % 3;

      int i1 = face.index[i];
      int i2 = face.index[ni];
      
      if (i1 > i2)
      {
        int thisEdge = edgeRef[i2 * vCount + i1];
        edgeArray[thisEdge].addSecondFace(it);
      }
    }

  }

  delete [] edgeRef;


  //
  // Normal calculations if a model does NOT include any normal data. This
  // code only does per-face normal solving since it's all that's needed
  // for non-normaled models in this assignment.
  //
  if (!hasNormals)
  {
    printf("Manually calculating normals.\n");

    for (int i = 0; i < vertArray.size(); i += 3)
    {
      Vec3& a = vertArray[i];
      Vec3& b = vertArray[i + 1];
      Vec3& c = vertArray[i + 2];

      // Calculate and normalize the vector.
      Vec3 n = crossProduct(a - b, b - c);
      n.unitize();
      
      for (int j = 0; j < 3; j++)
        normArray.push_back(n);
    }

    hasNormals = true;
  }

  calcFaceNormals();
}

void ObjModel::useTexture(const char *file)
{
  if (strlen(file) > 0)
  {
    string str(file);
    
    str = "data/textures/" + str + ".png";

    printf("Model using texture: %s\n", str.c_str());
    
    tex = new Texture();
    tex->loadTexture(str.c_str());
  }
}

//
// Main error function that will be called in case of a parse error.
//
void yyerror(ObjModel *objFile, ObjLoadData *ld, const char *s)
{
  fprintf(stderr, "%d: %s\n", yylineno, s);
}

