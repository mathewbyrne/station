//
// obj.h
//
// A sub-class of the Model object. The ObjModel loads in mesh data from an
// OBJ file.
//
// TODO: Need to add beter material support.
//

#ifndef _OBJ_H_
#define _OBJ_H_


#include "../model/model.h"
#include "../math/vec3.h"
#include <string>
using std::string;


typedef vector<Vec3> Vec3Array;


//
// TODO: This class doesn't actually do anything yet! Will add in support
// for named sections of meshes.
//
/*
class NamedObject
{
public:
  NamedObject(const char *v_name, int v_index);
  virtual ~NamedObject();

  NamedObject *next;
  UseMaterial *useMaterial;
  char *name;
  int index;
};*/


class ObjLoadData
{
public:
  Vec3Array vertices;
  Vec3Array texCoords;
  Vec3Array normals;

  Face *edgeRef;
};

//
// An OBJ loading Model class.
//
class ObjModel : public Model
{

public:

  ObjModel(const string& filename = "");
  virtual ~ObjModel(void);

  int loadFile(const string& filename);

  void findBoundingBox(Vec3 &min, Vec3 &max) const;

protected:

  void beginFace (void);

  void addFaceVertex (const int& vIndex, const int& tIndex,
      const int& nIndex, const ObjLoadData& ld);

  void finishFace (void);
  
  void postProcessModel (ObjLoadData& ld);

  int faceVertNo;

  void useTexture(const char *file);

private:

  string filename;

  friend int yyparse(ObjModel *objModel, ObjLoadData *ld);

};

#endif // _OBJ_H_

