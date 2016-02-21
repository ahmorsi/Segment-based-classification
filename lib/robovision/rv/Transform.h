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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cstdio>
#include <iostream>
#include <eigen3/Eigen/Dense>

#include "geometry.h"

namespace rv
{

/** \brief representation of a homogeneous transformation
 *
 *  The implementation explicitly stores the inverse transformation to save
 *  the overhead of computation of the inverse.
 *
 *  For further information:
 *    Matt Pharr, Greg Humphreys.
 *    Physically Based Rendering - From Theory to Implementation.
 *    Morgan Kaufmann, 2010.
 *
 *  \author behley
 **/
class Transform
{
  public:
    Transform()
    {
      m = Eigen::Matrix4f::Identity();
      mInv = Eigen::Matrix4f::Identity();
    }

    Transform(const float mat[4][4])
    {
      m << mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3];
      mInv = m.inverse();
    }

    Transform(const Eigen::Matrix4f& mat) :
        m(mat)
    {
      mInv = m.inverse();
    }

    Transform(const Eigen::Matrix4f& mat, const Eigen::Matrix4f& minv) :
        m(mat), mInv(minv)
    {
    }

    Transform(float t00, float t01, float t02, float t03, float t10, float t11,
        float t12, float t13, float t20, float t21, float t22, float t23,
        float t30, float t31, float t32, float t33)
    {
      m << t00, t01, t02, t03, t10, t11, t12, t13, t20, t21, t22, t23, t30, t31, t32, t33;
      mInv = m.inverse();
    }

    /** \brief construct homogeneous transformation from rotation and translation
     *
     *  Exploit that M = (R|t) and its Inverse(M) = (R^T|-R^T*t)
     **/
    Transform(const Eigen::Matrix3f& R, const Vector3f& t) :
        m(Eigen::Matrix4f::Identity()), mInv(Eigen::Matrix4f::Identity())
    {
      for (uint32_t i = 0; i < 3; ++i)
        for (uint32_t j = 0; j < 3; ++j)
          m(i, j) = R(i, j);

      /** the inverted matrix is given by the transpose of m ... **/
      mInv = m.transpose();

      Vector3f t_inv(
          mInv(0, 0) * t.x() + mInv(0, 1) * t.y() + mInv(0, 2) * t.z(),
          mInv(1, 0) * t.x() + mInv(1, 1) * t.y() + mInv(1, 2) * t.z(),
          mInv(2, 0) * t.x() + mInv(2, 1) * t.y() + mInv(2, 2) * t.z());

      m(0, 3) = t.x();
      m(1, 3) = t.y();
      m(2, 3) = t.z();

      /** ... and the inverse translation **/
      mInv(0, 3) = -t_inv.x();
      mInv(1, 3) = -t_inv.y();
      mInv(2, 3) = -t_inv.z();
    }

    /** \brief initialize transform from Yaw, Pitch, Roll angles, and a translation vector.
     *
     * There are two convention for this conversion possible:
     *  0 = Yaw-Pitch-Roll
     *  1 = Yaw-Roll -Pitch
     *
     *  (Note: assuming a right-handed coordinate system:
     *      yaw    rotates around the z axis,
     *      pitch  rotates around the y axis,
     *  and roll   rotates around the x axis.)
     **/
    Transform(float yaw, float pitch, float roll, const Vector3f& t,
        int convention = 0);

    operator const float*() const;

    friend Transform Inverse(const Transform& t)
    {
      return Transform(t.mInv, t.m);
    }

    friend Transform Transpose(const Transform& t)
    {
      return Transform(t.m.transpose(), t.mInv.transpose());
    }

    bool operator==(const Transform& t) const
    {
      return t.m == m && t.mInv == mInv;
    }

    bool operator!=(const Transform& t) const
    {
      return t.m != m || t.mInv != mInv;
    }

    bool operator<(const Transform& t2) const
    {
      for (uint32_t i = 0; i < 4; ++i)
        for (uint32_t j = 0; j < 4; ++j)
        {
          if (m(i, j) < t2.m(i, j)) return true;
          if (m(i, j) > t2.m(i, j)) return false;
        }
      return false;
    }

    bool IsIdentity() const
    {
      return (m(0, 0) == 1.f && m(0, 1) == 0.f && m(0, 2) == 0.f
          && m(0, 3) == 0.f && m(1, 0) == 0.f && m(1, 1) == 1.f
          && m(1, 2) == 0.f && m(1, 3) == 0.f && m(2, 0) == 0.f
          && m(2, 1) == 0.f && m(2, 2) == 1.f && m(2, 3) == 0.f
          && m(3, 0) == 0.f && m(3, 1) == 0.f && m(3, 2) == 0.f
          && m(3, 3) == 1.f);
    }

    const Eigen::Matrix4f& GetMatrix() const
    {
      return m;
    }

    const Eigen::Matrix4f& GetInverseMatrix() const
    {
      return mInv;
    }

    bool HasScale() const
    {
      float la2 = (*this)(Vector3f(1, 0, 0)).LengthSquared();
      float lb2 = (*this)(Vector3f(0, 1, 0)).LengthSquared();
      float lc2 = (*this)(Vector3f(0, 0, 1)).LengthSquared();
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
      return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
#undef NOT_ONE
    }
    inline Point3f operator()(const Point3f &pt) const;
    inline void operator()(const Point3f &pt, Point3f *ptrans) const;
    inline Vector3f operator()(const Vector3f &v) const;
    inline void operator()(const Vector3f &v, Vector3f *vt) const;
    inline Normal3f operator()(const Normal3f &) const;
    inline void operator()(const Normal3f &, Normal3f *nt) const;
    Transform operator*(const Transform &t2) const;
    bool SwapsHandedness() const;

    friend std::ostream& operator<<(std::ostream& out, const Transform& t)
    {
      out.width(4);
      out.precision(3);
      const Eigen::Matrix4f& m = t.GetMatrix();

      out << m;

      return out;
    }
  private:
    // Transform Private Data
    Eigen::Matrix4f m, mInv;
};

/** \brief returns the translation of a transform **/
Vector3f Translation(const Transform& t);
/** \brief returns a translation by delta **/
Transform Translate(const Vector3f& delta);
/** \brief returns a scaling by x, y, z **/
Transform Scale(float x, float y, float z);
/** \brief returns a rotation about the x axis. **/
Transform RotateX(float angle);
/** \brief returns a rotation about the y axis. **/
Transform RotateY(float angle);
/** \brief returns a rotation about the z axis. **/
Transform RotateZ(float angle);
/** \brief returns a rotation about the given axis with given angle. **/
Transform Rotate(float angle, const Vector3f& axis);

// Transform Inline Functions
inline Point3f Transform::operator()(const Point3f &pt) const
{
  Eigen::Vector4f res = m * pt.vec;
  res /= res[3];

  return Point3f(res[0], res[1], res[2]);
}

inline void Transform::operator()(const Point3f& pt, Point3f* ptrans) const
{
  ptrans->vec = m * pt.vec;
  ptrans->vec /= ptrans->vec[3];
}

inline Vector3f Transform::operator()(const Vector3f& v) const
{
  Eigen::Vector4f res = m * v.vec;

  return Vector3f(res[0], res[1], res[2]);
}

inline void Transform::operator()(const Vector3f& v, Vector3f* vt) const
{
  vt->vec = m * v.vec;
  vt->vec[3] = 0.0f;
}

inline Normal3f Transform::operator()(const Normal3f &n) const
{
  Eigen::Vector4f res = n.vec.transpose() * mInv; //(M^-1^T*n == (n^T*M^-1)^T)

  return Normal3f(res[0], res[1], res[2]);
}

inline void Transform::operator()(const Normal3f& n, Normal3f* nt) const
{
  nt->vec = n.vec.transpose() * mInv; //(M^-1^T*n == (n^T*M^-1)^T)
  nt->vec.transposeInPlace();
  nt->vec[3] = 0.0f;
}


inline Transform::operator const float*() const
{
  return m.data();
}

}

#endif // TRANSFORM_H
