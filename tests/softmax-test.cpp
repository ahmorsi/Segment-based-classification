#include <gtest/gtest.h>
#include <rv/Random.h>

#include "../project/L2SoftmaxObjective.h"

using namespace rv;

namespace
{

// check the computed gradient with the numerical gradient
TEST(SoftmaxRegressionTest, GradientTest)
{
  const uint32_t D = 5;
  const uint32_t K = 3;
  const uint32_t N = 500;

  Random rand(1329);
  std::vector<std::vector<float> > X;
  std::vector<uint16_t> Y;
  std::vector<float> W;

  X.reserve(N);
  Y.reserve(N);
  W.reserve(N);

  for (uint32_t i = 0; i < N; ++i)
  {
    std::vector<float> feature(D);

    for (uint32_t d = 0; d < D; ++d)
      feature[d] = rand.getGaussianFloat() * 5.0f + 10.0f;

    W.push_back(rand.getFloat());
    Y.push_back(i % K);
    X.push_back(feature);
  }

  L2SoftmaxObjective loss(X, Y);

  const uint32_t n = K * (D + 1);
  Eigen::VectorXd x(n);
  for (uint32_t i = 0; i < n; ++i)
    x[i] = rand.getFloat() + 10;

  L2SoftmaxObjective loss_reg(X, Y, 0.1);

  double threshold = 0.00001;

  ASSERT_TRUE(check_grad(loss, x) < threshold)<< "Difference of analytical and numerical gradient should be less than " << threshold << ", but is "<<check_grad(loss, x);
  ASSERT_TRUE(check_grad(loss_reg, x) < threshold)<< "Difference of analytical and numerical gradient should be less than " << threshold << ", but is "<<check_grad(loss_reg, x);
}

}
