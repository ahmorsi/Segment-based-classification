// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Random.h"

namespace rv
{

Random::Random() :
    rng_()
{

}

Random::Random(int32_t seed) :
    rng_(seed)
{

}

int32_t Random::operator()(int32_t n)
{
  return getInt(n);
}

int32_t Random::getInt(int32_t n)
{
  boost::uniform_int<> dist(0, n-1);

  return dist(rng_);
}

float Random::getFloat()
{
  return uniform01_(rng_);
}

float Random::getGaussianFloat()
{
  boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > gen(rng_, normal_);
  return gen();
}

}
