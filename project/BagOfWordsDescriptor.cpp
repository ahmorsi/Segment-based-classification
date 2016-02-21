#include "BagOfWordsDescriptor.h"

#include <cmath>

using namespace rv;

BagOfWordsDescriptor::BagOfWordsDescriptor(const ParameterList& params, const PointDescriptor& descriptor,
    const std::vector<std::vector<float> >& vocabulary) :
    SegmentDescriptor(params), descriptor_(descriptor.clone()), vocabulary_(vocabulary)
{
  normalizer_ = getNormalizerByName(params_["normalizer"]);
}

BagOfWordsDescriptor::~BagOfWordsDescriptor()
{
  delete descriptor_;
  delete normalizer_;
}

BagOfWordsDescriptor::BagOfWordsDescriptor(const BagOfWordsDescriptor& other) :
    SegmentDescriptor(other.params_), descriptor_(other.descriptor_->clone()), normalizer_(other.normalizer_->clone()), vocabulary_(
        other.vocabulary_)
{

}

BagOfWordsDescriptor& BagOfWordsDescriptor::operator=(const BagOfWordsDescriptor& other)
{
  delete descriptor_;
  delete normalizer_;

  params_ = other.params_;
  descriptor_ = other.descriptor_->clone();
  normalizer_ = other.normalizer_->clone();

  return *this;
}

BagOfWordsDescriptor* BagOfWordsDescriptor::clone() const
{
  return new BagOfWordsDescriptor(*this);
}

uint32_t BagOfWordsDescriptor::dim() const
{
  return vocabulary_.size();
}

void BagOfWordsDescriptor::evaluate(float* values, const IndexedSegment& segment, const Laserscan& scan,
    const NearestNeighborImpl& nn) const
{
  memset(values, 0, sizeof(float) * vocabulary_.size());
  Normal3f upvector(0.f, 0.f, 1.f); // use up-vector for computation of point descriptors.
  std::vector<float> pfeature(descriptor_->dim());

  for(int i = 0; i < segment.indexes.size(); i ++)
  {
	  float * v = pfeature.data();

	  descriptor_->evaluate(v, scan.point(segment.indexes[i]), upvector, scan, nn);
	  float mn = INT_MAX;
	  int index = -1;
	  for(int j = 0; j < vocabulary_.size(); j ++)
	  {
		   float d = distance(pfeature, vocabulary_[j]);
		   if(d < mn)
		   {
			   mn = d;
			   index = j;
		   }
	  }
	  ++ values[index];
  }

  normalizer_->normalize(values, vocabulary_.size());
}

// Determine for each point in segment the descriptor and find nearest word in dictionary.
float BagOfWordsDescriptor::distance(const std::vector<float>& a, const std::vector<float>& b) const
{
  float distance = 0.0f;
  for (uint32_t i = 0; i < a.size(); ++i)
    distance += std::pow(a[i] - b[i], 2);

  return distance;
}

