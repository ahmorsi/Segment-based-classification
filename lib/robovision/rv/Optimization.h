// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef OPTIMIZATION_H_
#define OPTIMIZATION_H_

#include <eigen3/Eigen/Dense>
#include "rv/ParameterList.h"
#include "Objective.h"
#include "OptimizationCallback.h"

namespace rv
{

/**
 * \brief Interface of an optimization algorithm for minimizing an objective.
 *
 * Note: The actual implementation should set some sensible parameters in the first place.
 *
 * \author behley
 */
class Optimization
{
  public:
    Optimization();
    virtual ~Optimization();

    /** \brief set parameters by replacing old parameters and keeping non-defined parameters in default state **/
    virtual void setParameters(const ParameterList& newparams);

    /** \brief return ParameterList with current parameters. **/
    virtual const ParameterList& getParameters() const;

    /** \brief minimizes f with initial value x0
     *
     *  returns a numerical code indicating the result of the minimization.
     *      0 - converged always means that the algorithm converged.
     *     < 0 - indicates some error, which can be translated by the method reason()
     **/
    virtual int32_t minimize(Objective& f, const Eigen::VectorXd& x0,
        OptimizationCallback* callback = 0) = 0;

    /** \brief returns the final function value at the minimum. **/
    virtual double value() const;

    /** \brief return the resulting optimal x, which minimizes the objective. **/
    virtual const Eigen::VectorXd& result() const;

    /** \brief returns a textual description of the error reason. **/
    virtual std::string reason(int32_t errorno) const = 0;

  protected:
    ParameterList params;
    Eigen::VectorXd xk;   // x_k at iteration k
    double fxk;           // f(x_k) at iteration k

};

}

#endif /* OPTIMIZATION_H_ */
