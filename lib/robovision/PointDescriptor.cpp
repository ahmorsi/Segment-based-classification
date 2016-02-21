// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/PointDescriptor.h"

namespace rv
{

PointDescriptor::PointDescriptor()
{

}

PointDescriptor::PointDescriptor(const ParameterList& params) :
    params_(params)
{

}

PointDescriptor::~PointDescriptor()
{

}

const ParameterList& PointDescriptor::params() const
{
  return params_;
}

}
