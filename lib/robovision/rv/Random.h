// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef RANDOM_H_
#define RANDOM_H_

#include <boost/random.hpp>

namespace rv
{

/** \brief random number generator
 *
 *  Essentially a wrapper for boost::random.
 *  
 *  \author behley
 */
class Random
{
  public:
    Random();
    Random(int32_t seed);

    int32_t operator()(int32_t n);

    /** \brief Returns a random integer value in the range [0, n-1]. */
    int32_t getInt(int32_t n);

    /** \brief Returns a random float in the range [0, 1]. */
    float getFloat();

    /** \brief Returns a random float from the Gaussian distribution at center 0 */
    float getGaussianFloat();

    /** \brief Returns the underlying random number generator. **/
    boost::mt19937& getGenerator();

    /** \brief get k samples from a given sequence.
     *
     *  Sampling without replacement from sequence.
     ***/
    template<typename T>
    std::vector<T> sample(const std::vector<T>& sequence, uint32_t k);

  protected:
    boost::mt19937 rng_;
    boost::uniform_01<> uniform01_;
    boost::normal_distribution<> normal_;
};

template<typename T>
std::vector<T> Random::sample(const std::vector<T>& sequence, uint32_t k)
{
  std::vector<T> copy(sequence); // working copy

  // for very large subset, we simple random_shuffle the sequence, otherwise use (Knuth-)Fisher-Yates shuffle.
  if (2 * k > sequence.size())
  {
    std::random_shuffle(copy.begin(), copy.end(), *this);
    copy.resize(k);
    return copy;
  }

  // (knuth-)fisher-yates shuffle.
  const uint32_t N = sequence.size();
  std::vector<T> result;
  result.reserve(k);
  for (uint32_t i = 0; i < k; ++i)
  {
    getInt(N - i);
    std::swap(copy[i], copy[N - i - 1]);
    result.push_back(copy[N - i - 1]);
  }

  return result;

}

} /* namespace rv */
#endif /* RANDOM_H_ */
