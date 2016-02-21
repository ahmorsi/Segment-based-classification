// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef OPTIMIZATIONCALLBACK_H_
#define OPTIMIZATIONCALLBACK_H_

#include <eigen3/Eigen/Dense>

 /** \brief callback object for minimize of Optimization.
  *
  *  \author behley
  **/
class OptimizationCallback
{
  public:
    virtual ~OptimizationCallback()
    {
    }
    /** \brief get the current function value fxk and state xk at iteration k. **/
    virtual void operator()(double fxk, const Eigen::VectorXd& xk) = 0;
};



#endif /* OPTIMIZATIONCALLBACK_H_ */
