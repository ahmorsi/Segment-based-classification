// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Optimization.h"

namespace rv
{

Optimization::Optimization() :
    fxk(-10000.0)
{
}

Optimization::~Optimization()
{
}

/** \brief set parameters by replacing old parameters and keeping non-defined parameters in default state **/
void Optimization::setParameters(const ParameterList& newparams)
{
  // replacing parameters, but keeping default values in the current state.
  for (ParameterList::const_iterator pit = newparams.begin();
      pit != newparams.end(); ++pit)
    params.insert(*pit);
}

/** \brief return ParameterList with current parameters. **/
const ParameterList& Optimization::getParameters() const
{
  return params;
}

/** \brief returns the final function value at the minimum. **/
double Optimization::value() const
{
  return fxk;
}

/** \brief return the resulting optimal x, which minimizes the objective. **/
const Eigen::VectorXd& Optimization::result() const
{
  return xk;
}

}
