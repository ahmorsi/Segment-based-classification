// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/ScaledCG.h"
#include "rv/Math.h"
#include "rv/PrimitiveParameters.h"

namespace rv
{

ScaledCG::ScaledCG()
{
  params.insert(IntegerParameter("max iterations", 200));
  params.insert(FloatParameter("sigma", 1e-06)); // <= 1e-04
  params.insert(FloatParameter("lambda", 1e-07)); // <= 1e-06
  params.insert(FloatParameter("stopping threshold", 1e-05));
  params.insert(IntegerParameter("past", 0));
  params.insert(FloatParameter("delta", 1e-5));
  params.insert(FloatParameter("factr", 1.0e+7));
}

void ScaledCG::setParameters(const ParameterList& newparams)
{
  // setting only parameters that are in the original parameter list.
  for (ParameterList::const_iterator it = newparams.begin(); it != newparams.end(); ++it)
  {
    if (params.hasParam(it->name())) params.insert(*it);
  }
}

int32_t ScaledCG::minimize(Objective& obj, const Eigen::VectorXd& x0, OptimizationCallback* callback)
{
  // getting the parameters.
  const uint32_t maxiter = params["max iterations"];
  double sigma = params["sigma"];
  double lambda = params["lambda"];
  const double epsilon = params["stopping threshold"];
  const double delta_thr = params["delta"];
  const double factr = params["factr"];
  const uint32_t past = params["past"];
  const double MACHEPS = 2.22045e-16;

  std::vector<double> pastf(past); // past values of fx

  uint32_t N = x0.rows();
  Eigen::VectorXd grad(N);

  xk = x0;
  fxk = obj(xk, grad);

  Eigen::VectorXd p = -grad;
  Eigen::VectorXd r = -grad;
  uint32_t k = 0;
  bool success = true;

//#sigma = 1e-06 # <= 1e-04
//#lamda = 1e-07 # <= 1e-06
  double lambda_bar = 0.0;

  double sigma_k = sigma;
  double delta = 0;

  for (;;)
  {
    double gnorm = grad.norm();
    double xnorm = std::max(1.0, xk.norm());

    /** check if we can stop here. **/
    if (gnorm / xnorm <= epsilon) break; // converged, hurray!
    if (maxiter > 0 && k >= maxiter) break; // max iterations reached. :/
    if (past > 0)
    {
      if (past <= k)
      {
        double rate = (pastf[k % past] - fxk) / fxk;

        /* The stopping criterion. */
        if (rate < delta_thr) break;

        double m = std::max(std::abs(pastf[k % past]), std::abs(fxk));
        m = std::max(m, 1.0);
        double change = (pastf[k % past] - fxk) / m;

        /* stopping criterion of Nocedal et al. (see algorithm.pdf) */
        if (change <= factr * MACHEPS) break; // converged, hurray!

      }

      pastf[k % past] = fxk; // store current function value.
    }

    double pnorm = p.norm();
    // 2. calculate second order information
    if (success)
    {
      sigma_k = sigma / pnorm;
      Eigen::VectorXd s(N);
      obj(xk + sigma_k * p, s);
      s = (s - grad) / sigma_k;
      delta = p.dot(s);
    }

    // 3. scale delta
    delta = delta + (lambda - lambda_bar) * (Math::sqr(pnorm));

    // 4. make Hessian matrix positive definite
    if (delta <= 0.)
    {
      lambda_bar = 2. * (lambda - delta / Math::sqr(pnorm));
      delta = -delta + lambda * Math::sqr(pnorm);
      lambda = lambda_bar;
    }

    // 5. calculate step size
    double mu = p.dot(r);
    double alpha = mu / delta;

    // 6. calculate comparison parameter
    double Delta = 2. * delta * (fxk - obj(xk + alpha * p)) / Math::sqr(mu);

    // 7. a successful reduction can be made.
    if (Delta >= 0)
    {
      Eigen::VectorXd x_kp1 = xk + alpha * p;

      fxk = obj(x_kp1, grad);

      Eigen::VectorXd r_old = r;
      r = -grad;

      lambda_bar = 0.;
      success = true;
      xk = x_kp1;
      if (k % N == 0)
      {
        // restart algorithm
        p = r;
      }
      else
      {
        double beta = (Math::sqr(r.norm()) - r.dot(r_old)) / mu;
        p = r + beta * p;
      }

      if (Delta >= 0.75)
      {
        // reduce the scale parameter
        lambda = 0.25 * lambda;
      }
    }
    else
    {
      lambda_bar = lambda;
      success = false;
    }

    // 8. increase scale parameter if needed.
    //pnorm = vecnorm(p)
    if (Delta < 0.25)
    {
      lambda = lambda + (delta * (1. - Delta) / Math::sqr(pnorm));
    }

    fxk = obj(xk);
    if (callback != 0) (*callback)(fxk, xk);

    ++k;
  }

//  if disp:
//  msg = "Optimization terminated successfully."
//  if k >= maxiter:
//  msg = "Maximum number of iterations reached."
//  elif gnorm > gtol:
//  msg = "Optimization not converged."
//  print(msg)
//  print("         Current function value: %f" % fval)
//  print("         Iterations: %d" % k)
//  print("         Function evaluations: %d" % func_calls[0])
//  print("         Gradient evaluations: %d" % grad_calls[0])

  if (maxiter > 0 && k >= maxiter) return -1; // reached maximum number of iters.

  return 0; // converged, everything fine.
}

std::string ScaledCG::reason(int32_t errorno) const
{
  switch (errorno)
  {
    case 0:
      return "No error: Converged.";
    case -1:
      return "Warning: Reached maximum number of iterations. Maybe increase number of iterations or increase stopping thresholds.";
  }

  return "Error undefined.";
}

}
