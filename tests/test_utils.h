#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <rv/NearestNeighborImpl.h>

/**
 * \brief Naive implementation of radius neighbor search.
 *
 * Simply searches for nearest neighbors by linearly search a list of points.
 *
 */
class NaiveNeighborSearch: public rv::NearestNeighborImpl
{
  public:
    void initialize(const std::vector<rv::Point3f>& points, const std::vector<uint32_t>& indexes);

    void radiusNeighbors(const rv::Point3f& query, float radius, std::vector<uint32_t>& resultIndices,
        const rv::Norm& norm) const;

    using rv::NearestNeighborImpl::initialize; // unhide implementation of base-class.
  protected:
    std::vector<uint32_t> indexes_;
    const std::vector<rv::Point3f>* data_;
};

bool almostEqualVectors(float* vec1, float* vec2, uint32_t n);

std::string stringify(float* v, uint32_t n);



#endif /* TEST_UTILS_H_ */
