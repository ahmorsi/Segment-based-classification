// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef SCALEDCG_H_
#define SCALEDCG_H_

#include "Optimization.h"
#include "Objective.h"
#include "ParameterList.h"

namespace rv
{

/** \brief Scaled Conjugate Gradient [1]
 *
 *  Checks the following convergence and stopping criteria:
 *    1. |g(x_k)| / \max(1, |x_k|) < stopping threshold   (convergence)
 *    2. (f(x_{k-past}) - f(x_k)) / f(x_k) < delta    (stopping) [past > 0 && k >= past]
 *    3. (f(x_{k-past}) - f(x_k)) / max(|f(x_{k-past})|, |f(x_k)|, 1.) < MACHEPS*factr (convergence) [past > 0 && k >= past]
 *
 *  [1] M. F. Moeller. A Scaled Conjugate Gradient Algorithm for Fast Supervised Learning.
 *      Neural Networks, 6(4), pp. 525--553, 1993.
 **/
class ScaledCG: public Optimization
{
  public:
    ScaledCG();

    void setParameters(const ParameterList& params);
    const ParameterList& getParameters();

    /** \brief minimizes f with initial value x0
     *
     *  returns a numerical code indicating the result of the minimization.
     *     0 - converged.
     *    -1 - maximum number of iterations reached.
     **/
    int32_t minimize(Objective& f, const Eigen::VectorXd& x0, OptimizationCallback* callback = 0);

    std::string reason(int32_t errorno) const;
};

} /* namespace rv */
#endif /* SCALEDCG_H_ */
