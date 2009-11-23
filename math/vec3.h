//
// vec3.h
//
// A very simple 3.5d vector class. Really only includes the w component for
// some of the lighting calculations. Does not use the w component for
// dot products, magnitude of a vector or anything else.
//


#ifndef _VEC3_H_
#define _VEC3_H_


#define SQ(x) ((x) * (x))
#include <cmath>



struct Vec3
{

  union
  {
    struct
    {
      float x, y, z, w;
    };

    float v[4];
  };

  Vec3 (void)
    : x(0.0), y(0.0), z(0.0), w(1.0)
  { }

  Vec3 (const float& x, const float& y, const float& z)
    : x(x), y(y), z(z), w(1.0)
  { }

  Vec3 (const float& x, const float& y, const float& z, const float& w)
    : x(x), y(y), z(z), w(w)
  { }

  Vec3 (const Vec3& v, const float& w)
    : w(w)
  {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  //
  // For array based access.
  //
  operator const float *(void)
  {
    return v;
  }


  //
  // For negating values.
  //
  Vec3 operator-() const
  {
    return Vec3(-x, -y, -z);
  }


  //
  //
  //
  const Vec3& operator=(const Vec3& copy)
  {
    x = copy.x;
    y = copy.y;
    z = copy.z;
    w = copy.w;

    return *this;
  }

  const Vec3& operator+=(const Vec3& right)
  {
    x += right.x;
    y += right.y;
    z += right.z;

    return *this;
  }

  const Vec3& operator*= (const float& right)
  {
    x *= right;
    y *= right;
    z *= right;

    return *this;
  }

  //
  // Returns the magnitude.
  //
  const float mag(void) const
  {
    return sqrt(SQ(x) + SQ(y) + SQ(z));
  }

  //
  // Returns a new unit lengthed vector.
  //
  const Vec3 unit(void) const
  {
    float thisMag = mag();
    if(thisMag == 0.0)
      return Vec3();
    return Vec3(x / thisMag, y / thisMag, z / thisMag);
  }

  //
  // Turns the current vector into a unit vector.
  //
  void unitize(void)
  {
    float thisMag = mag();
    if(thisMag == 0.0)
      return;
    
    x /= thisMag;
    y /= thisMag;
    z /= thisMag;
  }

  //
  //  Boolean to check for a 0 length vector.
  //
  const bool zero(void) const
  {
    return (x == y == z == 0.0);
  }

  const Vec3 operator* (const float& scale)
  {
     return Vec3(x * scale, y * scale, z * scale);
  }
};


//
// Returns the dot product of 2 vectors.
//
static float dot (const Vec3& a, const Vec3& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}


//
// Returns the cross product of 2 vectors.
//
static Vec3 crossProduct (const Vec3& a, const Vec3& b)
{
  return Vec3(a.y * b.z - a.z * b.y,
              a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}


static const Vec3 operator+(const Vec3& a, const Vec3& b)
{
  return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static const Vec3 operator-(const Vec3& a, const Vec3& b)
{
  return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static const Vec3 operator*(const float& a, const Vec3& b)
{
  return Vec3(a * b.x, a * b.y, a * b.z);
}


#endif // _VEC3_H_

