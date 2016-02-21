// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/SegmentDescriptor.h"

namespace rv
{

SegmentDescriptor::SegmentDescriptor(const ParameterList& params)
: params_(params)
{

}

SegmentDescriptor::~SegmentDescriptor()
{

}

const ParameterList& SegmentDescriptor::params() const
{
  return params_;
}

} /* namespace rv */
