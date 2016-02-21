// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef NORMS_H_
#define NORMS_H_

#include <cmath>

namespace rv
{

/** \brief definition of standard norms
 * 
 *  \author behley
 */
class Norm
{
  public:
    virtual ~Norm()
    {
    }
    virtual float compute(float x, float y, float z) const = 0;
    virtual float compute(const Point3f& a, const Point3f& b) const
    {
      float diff1 = a.x() - b.x();
      float diff2 = a.y() - b.y();
      float diff3 = a.z() - b.z();

      return compute(diff1, diff2, diff3);
    }
};

class ManhattenNorm: public Norm
{
  public:
    float compute(float x, float y, float z) const
    {
      return std::abs(x) + std::abs(y) + std::abs(z);
    }
};

class EuclideanNorm: public Norm
{
  public:
    float compute(float x, float y, float z) const
    {
      return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
    }
};

class MaximumNorm: public Norm
{
  public:
    float compute(float x, float y, float z) const
    {
      float maximum = std::max(std::abs(x), std::abs(y));
      maximum = std::max(maximum, std::abs(z));

      return maximum;
    }
};

}

#endif /* NORMS_H_ */
