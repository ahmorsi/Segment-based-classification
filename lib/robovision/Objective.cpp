// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include <stdint.h>

#include "rv/Objective.h"
#include "rv/Math.h"


namespace rv
{

double check_grad(Objective& obj, const Eigen::VectorXd& x0, double eps)
{
  double res = 0.0;
  uint32_t N = x0.rows();

  Eigen::VectorXd grad = x0;
  double fx = obj(x0, grad);

  // Compute finite-difference approximation of the gradient of a scalar function
  for (uint32_t i = 0; i < N; ++i)
  {
    Eigen::VectorXd xp = x0;
    xp[i] += eps;
    double fxp = obj(xp);

    Eigen::VectorXd xm = x0;
    xm[i] -= eps;
    double fxm = obj(xm);

    res += Math::sqr((0.5 * (fxp - fxm) / eps) - grad[i]);
  }

  return std::sqrt(res);
}

}
