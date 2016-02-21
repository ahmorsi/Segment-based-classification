#include <gtest/gtest.h>

#include <rv/PrimitiveParameters.h>
#include <rv/string_utils.h>
#include <rv/PointDescriptor.h>

#include "test_utils.h"
#include "../project/utils.h"
#include "../project/BagOfWordsDescriptor.h"

using namespace rv;

namespace
{

class SimpleDescriptor: public PointDescriptor
{
  public:
    SimpleDescriptor* clone() const
    {
      return new SimpleDescriptor(*this);
    }

    void evaluate(float* values, const Point3f& p, const Normal3f&, const Laserscan&,
        const NearestNeighborImpl& nn) const
    {
      std::vector<uint32_t> neighbors;
      nn.radiusNeighbors(p, 1.0, neighbors, MaximumNorm());
      values[0] = neighbors.size();
    }

    uint32_t dim() const
    {
      return 1;
    }
};

TEST(BagOfWordsTest, Compute)
{
  // some initialization work:

  Laserscan scan;
  ASSERT_NO_THROW(readLaserscan("data/train/000053.bin", scan))<< "expected laser scan in directory 'data/train/";

  const uint32_t D = 20;
  std::vector<std::vector<float> > vocabulary;
  for (uint32_t i = 0; i < D; ++i)
  {
    std::vector<float> word(1, i);
    vocabulary.push_back(word);
  }

  SimpleDescriptor descriptor;
  NaiveNeighborSearch nn;
  nn.initialize(scan.points());

  IndexedSegment segment;
  segment.indexes.push_back(12);
  segment.indexes.push_back(15);
  segment.indexes.push_back(18);
  segment.indexes.push_back(19);
  segment.indexes.push_back(27);
  segment.indexes.push_back(28);
  segment.indexes.push_back(29);

  ParameterList params;
  params.insert(StringParameter("normalizer", "none"));
  std::vector<float> bow(D);
  BagOfWordsDescriptor bow_descriptor(params, descriptor, vocabulary);

  bow_descriptor.evaluate(&bow[0], segment, scan, nn);

  float gold_bow[20] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 3 };

  ASSERT_TRUE(almostEqualVectors(gold_bow, &bow[0], 20));
}

}
