//
// model.cpp
//
// Most of the model functions are not that complex and are contained inside
// the header file. These functions here relate mostly to the shadow volume
// determination.
//


#include "model.h"
#include "../material/texture.h"
#include "../global.h"


//
// Returns a Vertex from the Model to which the edge belongs.
//
Vec3& Edge::getVertex (const int& which) const
{
  if (which)
    return model->realVerts[v1];
  else
    return model->realVerts[v2];
}


//
// Returns a reference to a Face object to which the edge has recorded.
//
Face& Edge::getFace (const int& which) const
{
  if (which)
    return model->faceArray[f1];
  else  
    return model->faceArray[f2];
}


//
// Destructor for cleaning various bits up.
//
Model::~Model ()
{
  if (usingVertexBuffers)
  {
    glDeleteBuffers(1, &vBuff);
    if (hasNormals)   glDeleteBuffers(1, &nBuff);
    if (hasTexCoords) glDeleteBuffers(1, &tBuff);
  }

  if (tex) delete tex;
}



//
// Loops through all faces in the current model and averages the faces normals.
// Doesn't check normal array bounds, assumes that the model Object is valid.
//
void Model::calcFaceNormals (void)
{
  for(vector<Face>::iterator it = faceArray.begin();
      it != faceArray.end(); ++it)
  {
    it->normal += normArray[it->vStart + 0];
    it->normal += normArray[it->vStart + 1];
    it->normal += normArray[it->vStart + 2];

    it->normal.unitize();
  }
}


//
// Should be called after the model has been loaded if you wish to use a
// VBO to draw the geometry. Since this assignment (and this class) is
// only dealing with static geometry, this is probably a good idea and
// big performance boost.
//
void Model::initVertexBuffers()
{
  usingVertexBuffers = true;

  // Create a large array, first half the real vertices, the second a copy of
  // the first with the w components set to 0.
  vector<Vec3> allVertArray = realVerts;
  for (int i = 0; i < realVerts.size(); ++i)
    allVertArray.push_back(Vec3(realVerts[i], 0.0f));
  
  // Vertex Array buffer.
  glGenBuffers(1, &vBuff);
  glBindBuffer(GL_ARRAY_BUFFER, vBuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * vertArray.size(),
      &(vertArray[0]), GL_STATIC_DRAW);

  // Extrusion array buffer.
  glGenBuffers(1, &eBuff);
  glBindBuffer(GL_ARRAY_BUFFER, eBuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * allVertArray.size(),
      &(allVertArray[0]), GL_STATIC_DRAW);

  // Normal array buffer.
  if (hasNormals)
  {
    glGenBuffers(1, &nBuff);
    glBindBuffer(GL_ARRAY_BUFFER, nBuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * normArray.size(),
        &(normArray[0]), GL_STATIC_DRAW);
  }

  // TexCoords array buffer.
  if (hasTexCoords)
  {
    glGenBuffers(1, &tBuff);
    glBindBuffer(GL_ARRAY_BUFFER, tBuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * textArray.size(),
        &(textArray[0]), GL_STATIC_DRAW);
  }
}


//
// Call this function to draw the mesh from a VBO. Will throw an exception if
// not initialised. This code should probably be in the renderer but it's a
// bit simpler here for my uses.
//
void Model::drawVertexBuffers()
{
  if (!usingVertexBuffers)
    throw std::runtime_error("Attempt to draw uninitialised VBOs.");
    
  glBindBuffer(GL_ARRAY_BUFFER, vBuff);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(4, GL_FLOAT, sizeof(Vec3), 0);

  if (hasNormals)
  {
    glBindBuffer(GL_ARRAY_BUFFER, nBuff);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(Vec3), 0);
  }

  if (hasTexCoords)
  {
    glBindBuffer(GL_ARRAY_BUFFER, tBuff);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vec3), 0);
  }

  if (tex)
  {
    glEnable(GL_TEXTURE_2D);
    tex->bindTexture();
  }

  glDrawArrays(GL_TRIANGLES, 0, vertArray.size());

  // Disable pointers if the were activated.
  if (hasNormals)   glDisableClientState(GL_NORMAL_ARRAY);
  if (hasTexCoords) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  if (tex)
  {
    glDisable(GL_TEXTURE_2D);
  }
}


//
// Since a buffer cannot be legally bound inside of glBegin/glEnd, it needs
// to be done prior in this function.
//
void Model::bindExtrudeBuffer ()
{
  glBindBuffer(GL_ARRAY_BUFFER, eBuff);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(4, GL_FLOAT, sizeof(Vec3), 0);
}


//
// Will draw an indexed quad from the Vertex Buffers. Remember that this can
// draw into the extruded values at twice the array length.
//
void Model::drawIndexQuad (const int& a, const int& b, const int& c,
    const int& d)
{
  if (!usingVertexBuffers)
    throw std::runtime_error("Attempt to draw uninitialised VBOs.");

  //TODO: Error check that these are within bounds.
  glBegin(GL_QUADS);
    glArrayElement(a);
    glArrayElement(b);
    glArrayElement(c);
    glArrayElement(d);
  glEnd();
}


//
// Same as above but only meant for a tri.
//
void Model::drawIndexTri (const int& a, const int& b, const int& c)
{
  if (!usingVertexBuffers)
    throw std::runtime_error("Attempt to draw uninitialised VBOs.");

  // TODO: Error check that these are within bounds.
  glBegin(GL_TRIANGLES);
    glArrayElement(a);
    glArrayElement(b);
    glArrayElement(c);
  glEnd();
}

