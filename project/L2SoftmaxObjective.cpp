/*
 * L2SoftmaxObjective.cpp
 *
 *  Created on: Jan 2, 2014
 *      Author: behley
 */

#include "L2SoftmaxObjective.h"
#include <rv/Math.h>
#include<cmath>
using namespace rv;

L2SoftmaxObjective::L2SoftmaxObjective(const std::vector<std::vector<float> >& features,
    const std::vector<uint16_t>& labels, float lambda) :
    X_(features), Y_(labels), lambda_(lambda)
{
  assert(X_.size() > 0);
  assert(X_.size() == Y_.size());

  K_ = Math::max(labels) + 1;
  N_ = X_.size();
  D_ = X_[0].size() + 1; // + bias weight
}

double L2SoftmaxObjective::kDot(const std::vector<float>& x, const Eigen::VectorXd& theta, uint32_t k)
{
  double result = 1.0 * theta[k * D_];

  for (uint32_t i = 0; i < D_ - 1; ++i)
    result += x[i] * theta[k * D_ + i + 1];

  return result;
}

double L2SoftmaxObjective::operator()(const Eigen::VectorXd& theta)
{
  Eigen::VectorXd P = Eigen::VectorXd::Zero(K_);
  Eigen::VectorXd a = Eigen::VectorXd::Zero(K_);
  double f = 0.0;

  double regularization = (theta.transpose().dot(theta))*lambda_/2;

  for(int idx=0;idx<N_;++idx)
  {
	  double norm = 0;
	  double z=0;
	  for(int k=0;k<K_;++k)
	  {
		  a(k) = kDot(X_[idx],theta,k);
		  z = std::max((double)a(k),z);
	  }
	  for(int k=0;k<K_;++k)
	  	  {
		  	  P(k) = std::exp((double)a(k)-z);
		  	  norm += P(k);
	  	  }
	  f -= (std::log((double)P(Y_[idx])/norm)-regularization);
  }
  f /= N_;
  return f;
}

double L2SoftmaxObjective::operator()(const Eigen::VectorXd& theta, Eigen::VectorXd& g)
{
  g = Eigen::VectorXd::Zero(K_ * D_);
  double f = 1.0;

  f = this->operator ()(theta);
  g[0] = 1000.0f;
  for(uint16_t j=0;j<K_;++j)
  {
	  Eigen::VectorXd gradientJ = CalculateGradient(j,theta);
	  int start = j*D_;
	  for(int idx=0;idx<D_;++idx)
	  {
		  g(idx + start)=gradientJ(idx);
	  }
  }
  return f;
}
Eigen::VectorXd L2SoftmaxObjective::CalculateGradient(uint16_t j,const Eigen::VectorXd& theta)
{
	Eigen::VectorXd grad = Eigen::VectorXd::Zero(D_);
	Eigen::VectorXd a = Eigen::VectorXd::Zero(K_);
	for(int idx=0;idx<N_;++idx)
	  {
		double z=0;
			  for(int k=0;k<K_;++k)
			  {
				  a(k) = kDot(X_[idx],theta,k);
				  z = std::max((double)a(k),z);
			  }
		double norm = 0,res;
		for(int k=0;k<K_;++k)
		  {
			norm += std::exp((double)a(k)-z);
		  }
		res = (j==Y_[idx]?1:0) - (std::exp((double)a(j)-z)/norm);
		grad(0) -= res-lambda_*theta(j*D_);
		for(int i=1;i<D_;++i)
		grad(i) -= (X_[idx][i-1]*res-lambda_*theta(j*D_ + i));
	  }
	for(int i=0;i<D_;++i)
	{
		grad(i) /= N_;
	}
	return grad;
}
