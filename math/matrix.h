//
// matrix.h
//
// A 4x4 Matrix class for handling world transformations. Since I didn't need
// any other matrix types in the assignment, I went with the name Matrix for
// the class.
// Also, since this is a datastructure more than a set of functions, all the
// class members are public for convinience.
//

#ifndef _MATRIX_H_
#define _MATRIX_H_


// Required for vector math.
#include "vec3.h"


// TODO: Consider moving this into a seperate math library.
#define MY_PI 3.1415926535897932385E0
#define RAD(x) ((x) * MY_PI / 180)


//
// Class definition and decleration.
//
class Matrix
{

public:

	// Data repretentation.
	union
	{
		// The matrix data is stored below. As I need it for OpenGL command, it's
		// stored as the transpose of what we're logically used to looking at.
		// (ie the rows are the columns).
		struct
		{
			float m11, m12, m13, l1;
			float m21, m22, m23, l2;
			float m31, m32, m33, l3;
			float  tx,  ty,  tz, tw;
		};
		
		// Storing this as an array as well as a set of values makes it very easy
		// to get OpenGL compatible arrays out.
		float values[16];
	};


	//
	// Default constructor, initialises the matrix to an identity.
	//
	Matrix (void)
  {
    identify();
  }

	
	//
	// Constructor from an array of float values. This assumes that a 16 element
	// float array is passed as the parameter. The basic idea is that OpenGL
	// compatible 'Matrix-arrays' can be used to initialise a Matrix object.
	// 
	Matrix (const float* initValues)
	{
		for (int i = 0; i < 16; i++)
			values[i] = initValues[i];
	}


	//
	// Set the current matrix to an identity matrix.
	//
	void identify (void)
	{
		m11 = 1.0f; m12 = 0.0f; m13 = 0.0f; l1 = 0.0f;
		m21 = 0.0f; m22 = 1.0f; m23 = 0.0f; l2 = 0.0f;
		m31 = 0.0f; m32 = 0.0f; m33 = 1.0f; l3 = 0.0f;
		 tx = 0.0f;  ty = 0.0f;  tz = 0.0f; tw = 1.0f;
	}

	
	//
	// Transforms a Vector by the current Matrix. Replaces the passed in Vector
	// with a newly transformed Vec3.
	//
	void transform (Vec3& v) const
	{
		v = Vec3 (
			(v.x * m11) + (v.y * m21) + (v.z * m31) + tx,
			(v.x * m12) + (v.y * m22) + (v.z * m32) + ty,
			(v.x * m13) + (v.y * m23) + (v.z * m33) + tz
	   );
	}
  
	
  // 
  // Used for array access to matrix elements.
  // 
  operator const float * (void) { return values; }


  //
  // Sets the translation component of the matrix.
  //
  void setTranslation (const Vec3& v)
  {
    tx = v.x; ty = v.y; tz = v.z;
  }


  //
  // Sets the current matrix to a rotation matrix with the rotations in the
  // X, Y and Z axis matching with the 3 values in the provided vector.
  //
  void fromAngles (const Vec3& rot)
  {
    float cx = cos(RAD(rot.x));
    float sx = sin(RAD(rot.x));
    float cy = cos(RAD(rot.y));
    float sy = sin(RAD(rot.y));
    float cz = cos(RAD(rot.z));
    float sz = sin(RAD(rot.z));

    float sxsy = sx * sy;
    float cxsy = cx * sy;

    // Solve the matrix values.
    m11 =  (cy * cz);
    m12 =  (sxsy * cz) + (cx * sz);
    m13 = -(cxsy * cz) + (sx * sz);

    m21 = -(cy * sz);
    m22 = -(sxsy * sz) + (cx * cz);
    m23 =  (cxsy * sz) + (sx * cz);

    m31 = sy;
    m32 = -(sx * cy);
    m33 =  (cx * cy);


    // Reset the translation component of the matrix.
    tx = ty = tz = 0.0f;
  }
};


//
// Returns a transpose of the supplied Matrix.
//
static Matrix transposeMatrix (const Matrix& m)
{
	Matrix ret;
	
	ret.m11 = m.m11; ret.m21 = m.m12; ret.m31 = m.m13; ret.tx = m.l1;
	ret.m12 = m.m21; ret.m22 = m.m22; ret.m32 = m.m23; ret.ty = m.l2;
	ret.m13 = m.m31; ret.m23 = m.m32; ret.m33 = m.m33; ret.tz = m.l3;
	ret.l1  = m.tx;  ret.l2  = m.ty;  ret.l3  = m.tz;  ret.tw = m.tw;

	return ret;
}


//
// Returns the determinate of the top 3x3 Matrix of a supplied 4x4 Matrix.
//
static float getMatrixDeterminant (const Matrix& m)
{
	return m.m11 * ((m.m22 * m.m33) - (m.m23 * m.m32))
			+ m.m12 * ((m.m23 * m.m31) - (m.m21 * m.m33))
			+ m.m13 * ((m.m21 * m.m32) - (m.m22 * m.m31));
}


//
// Returns the inverse of the supplied Matrix.
//
static Matrix invertMatrix (const Matrix& m)
{

  float det = getMatrixDeterminant(m);

	// TODO: Check determinate != 0.

	Matrix ret;
  float oneOverDet = 1.0f / det;

	// Solve the 3x3 Inverse part of the Matrix.
  ret.m11 = ((m.m22 * m.m33) - (m.m23 * m.m32)) * oneOverDet;
  ret.m12 = ((m.m13 * m.m32) - (m.m12 * m.m33)) * oneOverDet;
  ret.m13 = ((m.m12 * m.m23) - (m.m13 * m.m22)) * oneOverDet;

  ret.m21 = ((m.m23 * m.m31) - (m.m21 * m.m33)) * oneOverDet;
  ret.m22 = ((m.m11 * m.m33) - (m.m13 * m.m31)) * oneOverDet;
  ret.m23 = ((m.m13 * m.m21) - (m.m11 * m.m23)) * oneOverDet;

  ret.m31 = ((m.m21 * m.m32) - (m.m22 * m.m31)) * oneOverDet;
  ret.m32 = ((m.m12 * m.m31) - (m.m11 * m.m32)) * oneOverDet;
  ret.m33 = ((m.m11 * m.m22) - (m.m12 * m.m21)) * oneOverDet;

  // Solve the translation part of the inverse Matrix.
  ret.tx = -((m.tx * ret.m11) + (m.ty * ret.m21) + (m.tz * ret.m31));
  ret.ty = -((m.tx * ret.m12) + (m.ty * ret.m22) + (m.tz * ret.m32));
  ret.tz = -((m.tx * ret.m13) + (m.ty * ret.m23) + (m.tz * ret.m33));

  return ret;
}


//
// Matrix Multiplication, used to combine 2 matrices into 1! Note that as
// always, Matrix Multiplication is NOT commutative so be careful.
//
static Matrix operator* (const Matrix& l, const Matrix& r)
{
  Matrix m;

  // Ok, here we go, being VERY careful.
  m.m11 = (l.m11 * r.m11) + (l.m21 * r.m12) + (l.m31 * r.m13);
  m.m12 = (l.m12 * r.m11) + (l.m22 * r.m12) + (l.m32 * r.m13);
  m.m13 = (l.m13 * r.m11) + (l.m23 * r.m12) + (l.m33 * r.m13);

  m.m21 = (l.m11 * r.m21) + (l.m21 * r.m22) + (l.m31 * r.m23);
  m.m22 = (l.m12 * r.m21) + (l.m22 * r.m22) + (l.m32 * r.m23);
  m.m23 = (l.m13 * r.m21) + (l.m23 * r.m22) + (l.m33 * r.m23);

  m.m31 = (l.m11 * r.m31) + (l.m21 * r.m32) + (l.m31 * r.m33);
  m.m32 = (l.m12 * r.m31) + (l.m22 * r.m32) + (l.m32 * r.m33);
  m.m33 = (l.m13 * r.m31) + (l.m23 * r.m32) + (l.m33 * r.m33);

  m.tx  = (l.m11 * r.tx) + (l.m21 * r.ty) + (l.m31 * r.tz) + l.tx;
  m.ty  = (l.m12 * r.tx) + (l.m22 * r.ty) + (l.m32 * r.tz) + l.ty;
  m.tz  = (l.m13 * r.tx) + (l.m23 * r.ty) + (l.m33 * r.tz) + l.tz;

  return m;
}


//
// Creates a translation matrix from a vector.
//
static Matrix getTranslationMatrix (const Vec3& trans)
{
  Matrix m;
  m.setTranslation(trans);
  return m;
}


#endif // _MATRIX_H_

