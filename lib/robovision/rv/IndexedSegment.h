// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <iostream>
#include <stdint.h>
#include <vector>
#include "geometry.h"
#include "IOError.h"

/** \brief index-based representation of a segment.
 *  
 *  \author behley
 */

namespace rv
{

class IndexedSegment
{
  public:
    IndexedSegment()
    {
    }

    IndexedSegment(const std::vector<uint32_t>& ind) :
        indexes(ind)
    {
    }

    inline uint32_t operator[](uint32_t idx) const
    {
      return indexes[idx];
    }

    inline uint32_t size() const
    {
      return indexes.size();
    }

    std::vector<uint32_t> indexes;
};

}

#endif /* SEGMENT_H_ */
