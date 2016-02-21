#include <gtest/gtest.h>
#include <rv/Random.h>
#include <rv/string_utils.h>
#include "../project/KMeans.h"

using namespace rv;

namespace
{

std::vector<std::vector<float> > generateData(uint32_t N, uint32_t D)
{
  std::vector<std::vector<float> > data;
  data.reserve(N);

  Random rand;
  for (uint32_t i = 0; i < N; ++i)
  {
    std::vector<float> vec(D);
    for (uint32_t j = 0; j < D; ++j)
      vec[j] = 10.0f * rand.getGaussianFloat();

    data.push_back(vec);
  }

  return data;
}

float distanceSqr(const std::vector<float>& a, const std::vector<float>& b)
{
  float d = 0.0f;
  for (uint32_t i = 0; i < a.size(); ++i)
    d += (a[i] - b[i]) * (a[i] - b[i]);

  return d;
}

TEST(KmeansTest, Cluster)
{
  const uint32_t N = 100;
  const uint32_t D = 10;
  const uint32_t C = 15;
  KMeans kmeans;

  std::vector<std::vector<float> > data = generateData(N, D);
  std::vector<std::vector<float> > centers = kmeans.cluster(data, C);

  ASSERT_EQ(C, centers.size());
  for (uint32_t i = 0; i < centers.size(); ++i)
    ASSERT_EQ(D, centers[i].size());

  // Check convergence of kmeans clustering algorithm.
  std::vector<uint32_t> assignment(N, 0);
  for (uint32_t i = 0; i < N; ++i)
  {
    float min_distance = distanceSqr(data[i], centers[0]);

    for (uint32_t j = 1; j < C; ++j)
    {
      float d = distanceSqr(data[i], centers[j]);

      if (d < min_distance)
      {
        min_distance = d;
        assignment[i] = j;
      }
    }
  }

  for (uint32_t j = 0; j < C; ++j)
  {
    std::vector<float> center(D, 0.0f);
    uint32_t M = 0;

    for (uint32_t i = 0; i < N; ++i)
    {
      if (assignment[i] != j) continue;
      M += 1;
      for (uint32_t k = 0; k < D; ++k)
        center[k] += data[i][k];
    }

    if (M == 0) std::cout << "Warning: cluster center without assigned points." << std::endl;
    // mean of assigned points should match the computed center.
    for (uint32_t k = 0; k < D; ++k)
    {
      ASSERT_LT(std::abs(1.f/M*center[k]-centers[j][k]), 0.001);
    }
  }
}

}
