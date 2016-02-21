#include "test_utils.h"
#include <sstream>

using namespace rv;

void NaiveNeighborSearch::initialize(const std::vector<Point3f>& points, const std::vector<uint32_t>& indexes)
{
  data_ = &points;
  indexes_ = indexes;
}

void NaiveNeighborSearch::radiusNeighbors(const Point3f& query, float radius, std::vector<uint32_t>& resultIndices,
    const Norm& norm) const
{
  const std::vector<Point3f>& pts = *data_;
  resultIndices.clear();

  for (uint32_t i = 0; i < indexes_.size(); ++i)
  {
    if (norm.compute(query, pts[indexes_[i]]) < radius)
    {
      resultIndices.push_back(i);
    }
  }
}

bool almostEqualVectors(float* vec1, float* vec2, uint32_t n)
{
  for (uint32_t i = 0; i < n; ++i)
    if (std::abs(vec1[i] - vec2[i]) > 0.0001) return false;

  return true;
}

std::string stringify(float* v, uint32_t n)
{
  std::stringstream sstr;

  sstr << "[";
  for (uint32_t i = 0; i < n; ++i)
    sstr << ((i > 0) ? "," : "") << v[i];
  sstr << "]";

  return sstr.str();
}
