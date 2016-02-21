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

#include "rv/Transform.h"
#include <cstring>
#include "rv/Math.h"

namespace rv
{

Vector3f Translation(const Transform& t)
{
  const Eigen::Matrix4f& m = t.GetMatrix();
  return Vector3f(m(0, 3), m(1, 3), m(2, 3));
}

Transform Translate(const Vector3f& delta)
{
  Eigen::Matrix4f m;
  m << 1, 0, 0, delta.x(), 0, 1, 0, delta.y(), 0, 0, 1, delta.z(), 0, 0, 0, 1;
  Eigen::Matrix4f minv;
  minv << 1, 0, 0, -delta.x(), 0, 1, 0, -delta.y(), 0, 0, 1, -delta.z(), 0, 0, 0, 1;

  return Transform(m, minv);
}

Transform Scale(float x, float y, float z)
{
  Eigen::Matrix4f m;
  m << x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1;
  Eigen::Matrix4f minv;
  minv << 1.f / x, 0, 0, 0, 0, 1.f / y, 0, 0, 0, 0, 1.f / z, 0, 0, 0, 0, 1;

  return Transform(m, minv);
}

Transform RotateX(float angle)
{
  float sin_t = sinf(Math::deg2rad(angle));
  float cos_t = cosf(Math::deg2rad(angle));
  Eigen::Matrix4f m;
  m << 1, 0, 0, 0, 0, cos_t, -sin_t, 0, 0, sin_t, cos_t, 0, 0, 0, 0, 1;
  return Transform(m, m.transpose());
}

Transform RotateY(float angle)
{
  float sin_t = sinf(Math::deg2rad(angle));
  float cos_t = cosf(Math::deg2rad(angle));
  Eigen::Matrix4f m;
  m << cos_t, 0, sin_t, 0, 0, 1, 0, 0, -sin_t, 0, cos_t, 0, 0, 0, 0, 1;
  return Transform(m, m.transpose());
}

Transform RotateZ(float angle)
{
  float sin_t = sinf(Math::deg2rad(angle));
  float cos_t = cosf(Math::deg2rad(angle));
  Eigen::Matrix4f m;
  m << cos_t, -sin_t, 0, 0, sin_t, cos_t, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
  return Transform(m, m.transpose());
}

Transform Rotate(float angle, const Vector3f& axis)
{
  Vector3f a = Normalize(axis);
  float s = sinf(Math::deg2rad(angle));
  float c = cosf(Math::deg2rad(angle));
  float m[4][4];

  m[0][0] = a.x() * a.x() + (1.f - a.x() * a.x()) * c;
  m[0][1] = a.x() * a.y() * (1.f - c) - a.z() * s;
  m[0][2] = a.x() * a.z() * (1.f - c) + a.y() * s;
  m[0][3] = 0;

  m[1][0] = a.x() * a.y() * (1.f - c) + a.z() * s;
  m[1][1] = a.y() * a.y() + (1.f - a.y() * a.y()) * c;
  m[1][2] = a.y() * a.z() * (1.f - c) - a.x() * s;
  m[1][3] = 0;

  m[2][0] = a.x() * a.z() * (1.f - c) - a.y() * s;
  m[2][1] = a.y() * a.z() * (1.f - c) + a.x() * s;
  m[2][2] = a.z() * a.z() + (1.f - a.z() * a.z()) * c;
  m[2][3] = 0;

  m[3][0] = 0;
  m[3][1] = 0;
  m[3][2] = 0;
  m[3][3] = 1;

  Eigen::Matrix4f mat;
  mat << m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3];
  return Transform(mat, mat.transpose());
}

Transform Transform::operator*(const Transform &t2) const
{
  Eigen::Matrix4f m1 = m * t2.m;
  Eigen::Matrix4f m2 = t2.mInv * mInv;

  return Transform(m1, m2);
}

bool Transform::SwapsHandedness() const
{
  float det = ((m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))) - (m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)))
      + (m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0))));
  return det < 0.f;
}

Transform::Transform(float yaw, float pitch, float roll, const Vector3f& t, int convention)
{
  float s1 = sinf(yaw), c1 = cosf(yaw);
  float s2 = sinf(pitch), c2 = cosf(pitch);
  float s3 = sinf(roll), c3 = cosf(roll);

  m = Eigen::Matrix4f::Identity();
  mInv = Eigen::Matrix4f::Identity();

  if (convention == 0) /** RotateZ(yaw)*RotateY(pitch)*RotateX(roll) **/
  {
    m(0, 0) = c1 * c2;
    m(0, 1) = c1 * s2 * s3 - s1 * c3;
    m(0, 2) = s1 * s3 + c1 * s2 * c3;
    m(1, 0) = s1 * c2;
    m(1, 1) = s1 * s2 * s3 + c1 * c3;
    m(1, 2) = s1 * s2 * c3 - c1 * s3;
    m(2, 0) = -s2;
    m(2, 1) = c2 * s3;
    m(2, 2) = c2 * c3;
  }
  else
  {
    m(0, 0) = c1 * c2 - s1 * s2 * s3;
    m(0, 1) = -s1 * c3;
    m(0, 2) = c1 * s2 + s1 * s3 * c2;
    m(1, 0) = c2 * s1 + c1 * s2 * s3;
    m(1, 1) = c1 * c3;
    m(1, 2) = s1 * s2 - c1 * s3 * c2;
    m(2, 0) = -s2 * c3;
    m(2, 1) = s3;
    m(2, 2) = c3 * c2;
  }

  /** the inverted matrix is given by the transpose of m ... **/
  mInv = m.transpose();

  Vector3f t_inv(mInv(0, 0) * t.x() + mInv(0, 1) * t.y() + mInv(0, 2) * t.z(),
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

}
