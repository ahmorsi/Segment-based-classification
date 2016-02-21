// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/BBox.h"

namespace rv
{

BBox::BBox()
{
}

BBox::BBox(const Transform& trans, const Vector3f& ext) :
    t(trans), extent(ext)
{

}

BBox::BBox(const Laserscan& scan, const rv::IndexedSegment& segment)
{
  if (segment.size() > 0)
  {
    const std::vector<Point3f>& pts = scan.points();

    Point3f min(pts[segment.indexes[0]].x(), pts[segment.indexes[0]].y(), pts[segment.indexes[0]].z());
    Point3f max(pts[segment.indexes[0]].x(), pts[segment.indexes[0]].y(), pts[segment.indexes[0]].z());

    for (uint32_t i = 0; i < segment.indexes.size(); ++i)
    {
      const Point3f& pt = pts[segment.indexes[i]];

      /** determine min, max **/
      min.x() = std::min(pt.x(), min.x());
      min.y() = std::min(pt.y(), min.y());
      min.z() = std::min(pt.z(), min.z());
      max.x() = std::max(pt.x(), max.x());
      max.y() = std::max(pt.y(), max.y());
      max.z() = std::max(pt.z(), max.z());
    }

    Vector3f extent(0.5 * (max.x() - min.x()), 0.5 * (max.y() - min.y()), 0.5 * (max.z() - min.z()));
    Eigen::Matrix4f m = Eigen::Matrix4f::Identity(), minv = Eigen::Matrix4f::Identity();
    m(0, 3) = min.x() + extent.x();
    m(1, 3) = min.y() + extent.y();
    m(2, 3) = min.z() + extent.z();
    minv(0, 3) = -m(0, 3);
    minv(1, 3) = -m(1, 3);
    minv(2, 3) = -m(2, 3);

    t = Transform(m, minv);
    this->extent = extent;
  }
}

bool BBox::inside(const Point3f& p) const
{
  Point3f pp(p.x(), p.y(), p.z());
  Point3f q = Inverse(t)(pp); /** transform point in bounding box coordinate system. **/

  /** epsilon test to avoid equal test with floats **/
  return ((std::abs(q.x()) - extent.x()) < 0.001 && (std::abs(q.y()) - extent.y()) < 0.001
      && (std::abs(q.z()) - extent.z()) < 0.001);
}

float distanceInPlane(const BBox& bbox)
{
  float x = bbox.t.GetMatrix()(0, 3);
  float y = bbox.t.GetMatrix()(1, 3);

  return std::sqrt(x * x + y * y);
}

}
