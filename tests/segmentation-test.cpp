#include <gtest/gtest.h>
#include <rv/PrimitiveParameters.h>

#include "../project/GridbasedSegmentation.h"
#include "../project/utils.h"

using namespace rv;

namespace
{

TEST(SegmentationTest, Segment)
{
  Laserscan scan;
  ASSERT_NO_THROW(readLaserscan("data/train/000053.bin", scan))<< "expected laser scan in directory 'data/train/";

  ParameterList params;
  params.insert(FloatParameter("resolution", 0.5));
  params.insert(FloatParameter("max distance", 20.0f));
  params.insert(FloatParameter("min height", 0.3f));
  params.insert(IntegerParameter("min points", 50));

  GridbasedSegmentation seg(params);

  std::vector<IndexedSegment> segments;
  seg.segment(scan, segments);

  ASSERT_TRUE(segments.size() > 0);
  std::vector<uint32_t> count(scan.size(), 0);

  for (uint32_t i = 0; i < segments.size(); ++i)
  {
    const IndexedSegment& segment = segments[i];
    ASSERT_GE(segment.size(), 50)<< "Found segment with less than 50 points!";
    float minz, maxz;
    for (uint32_t j = 0; j < segment.size(); ++j)
    {
      uint32_t idx = segment[j];
      ASSERT_LE(count[idx], 1)<< "point " << idx << " already assigned to different segment.";
      count[idx] += 1;
      const Point3f& p = scan.point(idx);

      if (j == 0)
      {
        minz = maxz = p.z();
      }
      else
      {
        minz = std::min(p.z(), minz);
        maxz = std::max(p.z(), maxz);
      }

      ASSERT_FALSE(p.x() > 20.0f);
      ASSERT_FALSE(p.y() > 20.0f);
    }
    // minimum height of segment should should exceed min height.
    ASSERT_FALSE((maxz-minz) < 0.3);
  }
}

}
