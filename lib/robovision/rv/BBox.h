// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef BBOX_H
#define BBOX_H

#include "Transform.h"
#include "geometry.h"
#include "Laserscan.h"
#include "IndexedSegment.h"

namespace rv
{

/** \brief Bounding Box
 *
 *  Representation of a bounding box with a given transformation towards the center and extents, i.e.,
 *  half side-length from this center.
 *
 *  \author behley
 */
class BBox
{
  public:
    BBox();
    BBox(const Transform& trans, const Vector3f& ext);
    BBox(const Laserscan& scan, const IndexedSegment& segment);

    bool inside(const Point3f& p) const;

    Transform t; /** transform to the midpoint. **/
    Vector3f extent; /** dimensions in bounding box coordinate system. **/
};

float distanceInPlane(const BBox& bbox);

}

#endif
