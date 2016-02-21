// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef SEGMENTATION_H_
#define SEGMENTATION_H_

#include <vector>
#include "Laserscan.h"
#include "IndexedSegment.h"

namespace rv
{

/** \brief interface for a segmentation algorithm
 *
 *  \author behley
 */
class Segmentation
{
  public:
    virtual ~Segmentation()
    {
    }

    virtual void segment(const Laserscan& scan, std::vector<IndexedSegment>& segments) = 0;
};

}

#endif /* SEGMENTATION_H_ */
