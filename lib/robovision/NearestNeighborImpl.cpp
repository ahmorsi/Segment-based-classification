// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/NearestNeighborImpl.h"

namespace rv
{

NearestNeighborImpl::~NearestNeighborImpl()
{

}

void NearestNeighborImpl::initialize(const std::vector<Point3f>& pts)
{
  std::vector<uint32_t> indexes(pts.size());
  for (uint32_t i = 0; i < pts.size(); ++i)
    indexes[i] = i;
  initialize(pts, indexes);
}

}
