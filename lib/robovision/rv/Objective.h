// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef OBJECTIVE_H_
#define OBJECTIVE_H_

#include <eigen3/Eigen/Dense>

namespace rv
{

/** \brief an objective function, which we want to optimize
 *
 *  Every instantiation of objective must at least provide an operator for calculating a function value and gradient.
 *  For most objectives the function value can be calculated alongside the gradient; hence, it is beneficial to have a
 *  function that provides both at the same time.
 *
 *  \author behley
 */
class Objective
{
  public:
    virtual ~Objective()
    {
    }

    /** \brief function value at x **/
    virtual double operator()(const Eigen::VectorXd& x)
    {
      // default implementation simply calls (x,grad),
      // but should be re-implemented if computation is somehow involved.
      Eigen::VectorXd grad = x;
      return (*this)(x, grad);
    }

    /** \brief function value at x and gradient at x **/
    virtual double operator()(const Eigen::VectorXd& x,
        Eigen::VectorXd& gradient) = 0;

};

/** \brief returns distance between computational and analytical gradient of objective function.
 *
 *  The difference between the computed and analytical gradient should be very small.
 **/
double check_grad(Objective& obj, const Eigen::VectorXd& x0, double eps =
    1.0e-4);

}

#endif /* OBJECTIVE_H_ */
