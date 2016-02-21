// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Classifier.h"

namespace rv
{

Classifier::Classifier() :
    nClasses_(0), callback_(0)
{

}

Classifier::Classifier(const Classifier& other) :
    params_(other.params_), nClasses_(other.nClasses_), callback_(other.callback_)
{

}

Classifier& Classifier::operator=(const Classifier& other)
{
  if (&other != this) return *this;

  nClasses_ = other.nClasses_;
  params_ = other.params_;
  callback_ = other.callback_;

  return *this;
}

Classifier::~Classifier()
{

}

void Classifier::setParameters(const ParameterList& params)
{
  // maintaining old parameters!
  for (ParameterList::const_iterator it = params.begin(); it != params.end(); ++it)
    params_.insert(*it);
}

const ParameterList& Classifier::getParameters() const
{
  return params_;
}

void Classifier::setCallback(OptimizationCallback* c)
{
  callback_ = c;
}

uint32_t Classifier::numClasses() const
{
  return nClasses_;
}

}
