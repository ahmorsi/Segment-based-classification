/*
 pbrt source code Copyright(c) 1998-2010 Matt Pharr and Greg Humphreys.

 This file is part of pbrt.

 pbrt is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.  Note that the text contents of
 the book "Physically Based Rendering" are *not* licensed under the
 GNU GPL.

 pbrt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

/**
 * Distinguish between points, vectors, and normals. Especially, normals need
 * non-standard implementation of transformations.
 *
 * The different entities provide some helper functions and also arithmetic operations
 * for basic linear algebra.
 *
 * In contrast to the original implementation of Pharr & Humphreys these classes use
 * Eigen vectors to represent the points, which enables to directly use Eigen for
 * advanced linear algebra computations (SVD, etc...)
 *
 * For further information:
 *    Matt Pharr, Greg Humphreys.
 *    Physically Based Rendering - From Theory to Implementation.
 *    Morgan Kaufmann, 2010.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cassert>
#include <cmath>
#include <stdint.h>
#include <iostream>
#include <eigen3/Eigen/Dense>

namespace rv
{

// forward declatrions
struct Vector3f;
struct Point3f;
struct Normal3f;

/** \brief representation of a vector and arithmetic operations **/
struct Vector3f
{
  public:
    // Vector3f public methods
    Vector3f() :
        vec(0.0f, 0.0f, 0.0f, 0.0f)
    {

    }

    Vector3f(float xx, float yy, float zz) :
        vec(xx, yy, zz, 0.0f)
    {
      assert(!HasNaNs());
    }

    bool HasNaNs() const
    {
      return isnan((double)vec[0]) || isnan((double)vec[1]) || isnan((double)vec[2]);
    }

    explicit Vector3f(const Point3f &p);

    Vector3f(const Vector3f &v) :
        vec(v.vec[0], v.vec[1], v.vec[2], 0.0f)
    {
      assert(!v.HasNaNs());
    }

    Vector3f &operator=(const Vector3f &v)
    {
      assert(!v.HasNaNs());
      vec = v.vec;

      return *this;
    }

    Vector3f operator+(const Vector3f &v) const
    {
      assert(!v.HasNaNs());
      Eigen::Vector4f res = vec + v.vec;

      return Vector3f(res[0], res[1], res[2]);
    }

    Vector3f& operator+=(const Vector3f &v)
    {
      assert(!v.HasNaNs());

      vec += v.vec;

      return *this;
    }

    Vector3f operator-(const Vector3f &v) const
    {
      assert(!v.HasNaNs());

      Eigen::Vector4f res = vec - v.vec;

      return Vector3f(res[0], res[1], res[2]);
    }

    Vector3f& operator-=(const Vector3f &v)
    {
      assert(!v.HasNaNs());

      vec -= v.vec;

      return *this;
    }
    Vector3f operator*(float f) const
    {
      Eigen::Vector4f res = f * vec;

      return Vector3f(res[0], res[1], res[2]);
    }

    Vector3f &operator*=(float f)
    {
      assert(!isnan(f));

      vec *= f;

      return *this;
    }

    Vector3f operator/(float f) const
    {
      assert(f != 0);
      float inv = 1.f / f;
      Eigen::Vector4f res = inv * vec;

      return Vector3f(res[0], res[1], res[2]);
    }

    Vector3f &operator/=(float f)
    {
      assert(f != 0);

      float inv = 1.f / f;
      vec *= inv;

      return *this;
    }

    Vector3f operator-() const
    {
      return Vector3f(-vec[0], -vec[1], -vec[2]);
    }

    float operator[](int32_t i) const
    {
      assert(i >= 0 && i <= 2);
      return vec[i];
    }

    float& operator[](int32_t i)
    {
      assert(i >= 0 && i <= 2);
      return vec[i];
    }

    inline const float& x() const
    {
      return vec[0];
    }

    inline float& x()
    {
      return vec[0];
    }

    inline const float& y() const
    {
      return vec[1];
    }

    inline float& y()
    {
      return vec[1];
    }

    inline const float& z() const
    {
      return vec[2];
    }

    inline float& z()
    {
      return vec[2];
    }

    float LengthSquared() const
    {
      return vec.squaredNorm();
    }

    float Length() const
    {
      return vec.norm();
    }

    explicit Vector3f(const Normal3f &n);

    bool operator==(const Vector3f &v) const
    {
      return vec == v.vec;
    }

    bool operator!=(const Vector3f &v) const
    {
      return vec != v.vec;
    }

    friend std::ostream& operator<<(std::ostream& out, const Vector3f& v)
    {
      out.width(4);
      out.precision(3);
      out << v.vec[0] << ", " << v.vec[1] << ", " << v.vec[2];
      return out;
    }

    // Vector3f Public Data
    Eigen::Vector4f vec;
};

struct Point3f
{
  public:
    // Point3f Public Methods
    Point3f() :
        vec(0.f, 0.f, 0.f, 1.f)
    {

    }

    Point3f(float xx, float yy, float zz) :
        vec(xx, yy, zz, 1.f)
    {
      assert(!HasNaNs());
    }

    Point3f(const Point3f &p) :
        vec(p.vec[0], p.vec[1], p.vec[2], 1.0f)
    {
      assert(!p.HasNaNs());
    }

    Point3f& operator=(const Point3f &p)
    {
      assert(!p.HasNaNs());

      vec = p.vec;
      vec[3] = 1.0;

      return *this;
    }

    Point3f operator+(const Vector3f& v) const
    {
      assert(!v.HasNaNs());
      Eigen::Vector4f res = vec + v.vec;

      return Point3f(res[0], res[1], res[2]);
    }

    Point3f& operator+=(const Vector3f &v)
    {
      assert(!v.HasNaNs());
      vec += v.vec;
      vec[3] = 1.0f;

      return *this;
    }

    Vector3f operator-(const Point3f &p) const
    {
      assert(!p.HasNaNs());
      Eigen::Vector4f res = vec - p.vec;

      return Vector3f(res[0], res[1], res[2]);
    }

    Point3f operator-(const Vector3f& v) const
    {
      assert(!v.HasNaNs());
      Eigen::Vector4f res = vec - v.vec;

      return Point3f(res[0], res[1], res[2]);
    }

    Point3f& operator-=(const Vector3f& v)
    {
      assert(!v.HasNaNs());

      vec -= v.vec;
      vec[3] = 1.0f;

      return *this;
    }

    Point3f& operator+=(const Point3f& p)
    {
      assert(!p.HasNaNs());

      vec += p.vec;
      vec[3] = 1.0f;

      return *this;
    }

    Point3f operator+(const Point3f& p) const
    {
      assert(!p.HasNaNs());

      Eigen::Vector4f res = vec + p.vec;
      return Point3f(res[0], res[1], res[2]);
    }

    Point3f operator*(float f) const
    {
      Eigen::Vector4f res = f * vec;

      return Point3f(res[0], res[1], res[2]);
    }

    Point3f& operator*=(float f)
    {
      vec *= f;
      vec[3] = 1.0f;

      return *this;
    }

    Point3f operator/(float f) const
    {
      assert(f != 0);

      float inv = 1.f / f;
      Eigen::Vector4f res = vec * inv;

      return Point3f(res[0], res[1], res[2]);
    }

    Point3f& operator/=(float f)
    {
      assert(f != 0);
      float inv = 1.f / f;

      vec *= inv;
      vec[3] = 1.0f;

      return *this;
    }

    float operator[](int32_t i) const
    {
      assert(i >= 0 && i <= 2);
      return vec[i];
    }

    float &operator[](int32_t i)
    {
      assert(i >= 0 && i <= 2);
      return vec[i];
    }

    inline const float& x() const
    {
      return vec[0];
    }

    inline float& x()
    {
      return vec[0];
    }

    inline const float& y() const
    {
      return vec[1];
    }

    inline float& y()
    {
      return vec[1];
    }

    inline const float& z() const
    {
      return vec[2];
    }

    inline float& z()
    {
      return vec[2];
    }

    bool HasNaNs() const
    {
      return isnan((double)vec[0]) || isnan((double)vec[1]) || isnan((double)vec[2]);
    }

    bool operator==(const Point3f &p) const
    {

      return vec == p.vec;
    }

    bool operator!=(const Point3f &p) const
    {
      return vec != p.vec;
    }

    friend std::ostream& operator<<(std::ostream& out, const Point3f& p)
    {
      out.width(4);
      out.precision(3);
      out << p.vec[0] << ", " << p.vec[1] << ", " << p.vec[2];
      return out;
    }

    // Point3f Public Data
    Eigen::Vector4f vec;
};

struct Normal3f
{
  public:
    // Normal3f Public Methods
    Normal3f() :
        vec(0.0f, 0.0f, 0.0f, 0.0f)
    {

    }

    Normal3f(float xx, float yy, float zz) :
        vec(xx, yy, zz, 0.0f)
    {
      assert(!HasNaNs());
    }

    Normal3f operator-() const
    {
      return Normal3f(-vec[0], -vec[1], -vec[2]);
    }

    Normal3f operator+(const Normal3f& n) const
    {
      assert(!n.HasNaNs());

      Eigen::Vector4f res = vec + n.vec;
      return Normal3f(res[0], res[1], res[2]);
    }

    Normal3f& operator+=(const Normal3f& n)
    {
      assert(!n.HasNaNs());

      vec += n.vec;

      return *this;
    }

    Normal3f operator-(const Normal3f& n) const
    {
      assert(!n.HasNaNs());

      Eigen::Vector4f res = vec - n.vec;
      return Normal3f(res[0], res[1], res[2]);
    }

    Normal3f& operator-=(const Normal3f& n)
    {
      assert(!n.HasNaNs());

      vec -= n.vec; // n.w == 0!

      return *this;
    }

    bool HasNaNs() const
    {
      return isnan((double)vec[0]) || isnan((double)vec[1]) || isnan((double)vec[2]);
    }

    Normal3f operator*(float f) const
    {
      Eigen::Vector4f res = f * vec;

      return Normal3f(res[0], res[1], res[2]);
    }

    Normal3f& operator*=(float f)
    {
      vec *= f; // vec.w == 0

      return *this;
    }

    Normal3f operator/(float f) const
    {
      assert(f != 0);
      float inv = 1.f / f;
      Eigen::Vector4f res = inv * vec;

      return Normal3f(res[0], res[1], res[2]);
    }

    Normal3f& operator/=(float f)
    {
      assert(f != 0);
      float inv = 1.f / f;
      vec *= inv;

      return *this;
    }

    float LengthSquared() const
    {
      return vec.squaredNorm();
    }

    float Length() const
    {
      return vec.norm();
    }

    Normal3f(const Normal3f& n) :
        vec(n.vec[0], n.vec[1], n.vec[2], 0.0f)
    {
      assert(!n.HasNaNs());
    }

    Normal3f& operator=(const Normal3f &n)
    {
      assert(!n.HasNaNs());
      vec = n.vec;

      return *this;
    }

    explicit Normal3f(const Vector3f& v) :
        vec(v.vec[0], v.vec[1], v.vec[2], 0.0f)
    {
      assert(!v.HasNaNs());
    }

    float operator[](int32_t i) const
    {
      assert(i >= 0 && i <= 2);
      return vec[i];
    }

    float &operator[](int32_t i)
    {
      assert(i >= 0 && i <= 2);
      return vec[i];
    }

    inline const float& x() const
    {
      return vec[0];
    }

    inline float& x()
    {
      return vec[0];
    }

    inline const float& y() const
    {
      return vec[1];
    }

    inline float& y()
    {
      return vec[1];
    }

    inline const float& z() const
    {
      return vec[2];
    }

    inline float& z()
    {
      return vec[2];
    }

    bool operator==(const Normal3f& n) const
    {
      return vec == n.vec;
    }
    bool operator!=(const Normal3f& n) const
    {
      return vec != n.vec;
    }

    // Normal3f Public Data
    Eigen::Vector4f vec;
};

// Geometry Inline Functions
inline Vector3f::Vector3f(const Point3f& p) :
    vec(p.vec[0], p.vec[1], p.vec[2], 0.0f)
{
  assert(!HasNaNs());
}

inline Vector3f operator*(float f, const Vector3f& v)
{
  return v * f;
}

inline float Dot(const Vector3f& v1, const Vector3f& v2)
{
  assert(!v1.HasNaNs() && !v2.HasNaNs());

  return v1.vec.dot(v2.vec) - v1.vec[3] * v2.vec[3]; // v1.w == v2.w == 1.0!
}

inline float AbsDot(const Vector3f& v1, const Vector3f& v2)
{
  assert(!v1.HasNaNs() && !v2.HasNaNs());

  return fabsf((float)(v1.vec.dot(v2.vec) - v1.vec[3] * v2.vec[3]));
}

inline Vector3f Cross(const Vector3f& v1, const Vector3f& v2)
{
  assert(!v1.HasNaNs() && !v2.HasNaNs());
  Eigen::Vector4f res = v1.vec.cross3(v2.vec);

  return Vector3f(res[0], res[1], res[2]);
}

inline Vector3f Cross(const Vector3f &v1, const Normal3f &v2)
{
  assert(!v1.HasNaNs() && !v2.HasNaNs());
  Eigen::Vector4f res = v1.vec.cross3(v2.vec);

  return Vector3f(res[0], res[1], res[2]);
}

inline Vector3f Cross(const Normal3f &v1, const Vector3f &v2)
{
  assert(!v1.HasNaNs() && !v2.HasNaNs());
  Eigen::Vector4f res = v1.vec.cross3(v2.vec);

  return Vector3f(res[0], res[1], res[2]);
}

inline Vector3f Normalize(const Vector3f& v)
{
  return v / v.Length();
}

inline float Distance(const Point3f& p1, const Point3f& p2)
{
  return (p1 - p2).Length();
}

inline float DistanceSquared(const Point3f& p1, const Point3f& p2)
{
  return (p1 - p2).LengthSquared();
}

inline Point3f operator*(float f, const Point3f& p)
{
  assert(!p.HasNaNs());
  return p * f;
}

inline Normal3f operator*(float f, const Normal3f& n)
{
  return Normal3f(f * n.vec[0], f * n.vec[1], f * n.vec[2]);
}

inline Normal3f Normalize(const Normal3f& n)
{
  return n / n.Length();
}

inline Vector3f::Vector3f(const Normal3f& n) :
    vec(n.vec[0], n.vec[1], n.vec[2], 0.0f)
{
  assert(!n.HasNaNs());
}

inline float Dot(const Normal3f& n1, const Vector3f& v2)
{
  assert(!n1.HasNaNs() && !v2.HasNaNs());

  return n1.vec.dot(v2.vec); // n1.w == 0!
}

inline float Dot(const Vector3f& v1, const Normal3f& n2)
{
  assert(!v1.HasNaNs() && !n2.HasNaNs());

  return v1.vec.dot(n2.vec); // n2.w == 0!
}

inline float Dot(const Normal3f& n1, const Normal3f& n2)
{
  assert(!n1.HasNaNs() && !n2.HasNaNs());

  return n1.vec.dot(n2.vec); // n2.w n == n1.w == 0!
}

inline float AbsDot(const Normal3f& n1, const Vector3f& v2)
{
  assert(!n1.HasNaNs() && !v2.HasNaNs());

  return fabsf(n1.vec.dot(v2.vec)); // n1.w == 0
}

inline float AbsDot(const Vector3f& v1, const Normal3f& n2)
{
  assert(!v1.HasNaNs() && !n2.HasNaNs());

  return fabsf(v1.vec.dot(n2.vec)); // n2.w == 0
}

inline float AbsDot(const Normal3f& n1, const Normal3f& n2)
{
  assert(!n1.HasNaNs() && !n2.HasNaNs());

  return fabsf(n1.vec.dot(n2.vec)); // n1.w == n2.w == 0
}

inline Normal3f Faceforward(const Normal3f& n, const Vector3f& v)
{
  return (Dot(n, v) < 0.f) ? -n : n;
}

inline Normal3f Faceforward(const Normal3f& n, const Normal3f& n2)
{
  return (Dot(n, n2) < 0.f) ? -n : n;
}

inline Vector3f Faceforward(const Vector3f& v, const Vector3f& v2)
{
  return (Dot(v, v2) < 0.f) ? -v : v;
}

inline Vector3f Faceforward(const Vector3f& v, const Normal3f& n2)
{
  return (Dot(v, n2) < 0.f) ? -v : v;
}

}
#endif // GEOMETRY_H
