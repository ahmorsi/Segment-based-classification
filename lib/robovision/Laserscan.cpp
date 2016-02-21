// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Laserscan.h"

namespace rv
{

Laserscan::Laserscan()
{

}

/** \brief clear **/
void Laserscan::clear()
{
  points_.clear();
  remissions_.clear();
  normals_.clear();
}

/** \brief getter for points/normals/remission **/
const Point3f& Laserscan::point(uint32_t i) const
{
  assert(i < points_.size());
  return points_[i];
}

float Laserscan::remission(uint32_t i) const
{
  assert(i < remissions_.size());
  return remissions_[i];
}

const Normal3f& Laserscan::normal(uint32_t i) const
{
  assert(i < normals_.size());
  return normals_[i];
}

Transform& Laserscan::pose()
{
  return pose_;
}

const Transform& Laserscan::pose() const
{
  return pose_;
}

uint32_t Laserscan::size() const
{
  return points_.size();
}

std::vector<Point3f>& Laserscan::points()
{
  return points_;
}

const std::vector<Point3f>& Laserscan::points() const
{
  return points_;
}

std::vector<float>& Laserscan::remissions()
{
  return remissions_;
}

const std::vector<float>& Laserscan::remissions() const
{
  return remissions_;
}

const std::vector<Normal3f>& Laserscan::normals() const
{
  return normals_;
}

std::vector<Normal3f>& Laserscan::normals()
{
  return normals_;
}

bool Laserscan::hasRemission() const
{
  return (points_.size() == remissions_.size());
}

bool Laserscan::hasNormals() const
{
  return (points_.size() == normals_.size());
}

}
