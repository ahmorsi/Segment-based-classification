#include <gtest/gtest.h>
#include <rv/Random.h>
#include <rv/PrimitiveParameters.h>
#include <rv/NearestNeighborImpl.h>
#include <rv/string_utils.h>

#include "../project/utils.h"
#include "../project/SpinImage.h"
#include "test_utils.h"

using namespace rv;

namespace
{

// check the computed gradient with the numerical gradient
TEST(SpinImageTest, Computation)
{
  Laserscan scan;
  ASSERT_NO_THROW(readLaserscan("data/train/000053.bin", scan))<< "expected laser scan in directory 'data/train/";

  ParameterList params;
  params.insert(IntegerParameter("num-bins", 3));
  params.insert(BooleanParameter("bilinear", false));
  params.insert(FloatParameter("radius", 0.5));
  params.insert(StringParameter("normalizer", "none"));

  SpinImage si(params);

  NaiveNeighborSearch nn;
  nn.initialize(scan.points());
  std::vector<float> feature(si.dim(), 0);
  ASSERT_EQ(feature.size(), 9);
  Normal3f upvector(0.0f, 0.0f, 1.0f);

  float v1[9] =
  { 0, 0, 0, 1, 1, 0, 0, 0, 0 };
  float v2[9] =
  { 1, 0, 0, 1, 0, 0, 0, 0, 0 };
  float v3[9] =
  { 0, 10, 13, 10, 8, 4, 0, 0, 0 };

  si.evaluate(&feature[0], scan.point(0), upvector, scan, nn);
  ASSERT_EQ(true, almostEqualVectors(v1, &feature[0], 9))<< "Expected: " << stringify(v1, 9) << ", but got: " << rv::stringify(feature);

  si.evaluate(&feature[0], scan.point(55), upvector, scan, nn);
  ASSERT_TRUE(almostEqualVectors(v2, &feature[0], 9))<< "Expected: " << stringify(v2, 9) << ", but got: " << rv::stringify(feature);

  si.evaluate(&feature[0], scan.point(103), upvector, scan, nn);
  ASSERT_TRUE(almostEqualVectors(v3, &feature[0], 9))<< "Expected: " << stringify(v2, 9) << ", but got: " << rv::stringify(feature);
}

}
