// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "SoftmaxRegression.h"
#include <rv/PrimitiveParameters.h>
#include <fstream>
#include <rv/Stopwatch.h>
#include <rv/CompositeParameter.h>
#include <rv/Parameter.h>
#include <rv/string_utils.h>
#include <rv/Math.h>
#include <rv/IOError.h>

#include "rv/ScaledCG.h"
#include "L2SoftmaxObjective.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace rv
{

SoftmaxRegression::SoftmaxRegression() :
    bias(1.0f), D(0), trained(false)
{
  /** default parameters. **/
  params_.insert(StringParameter("optimization", "scg"));
  params_.insert(FloatParameter("lambda", 0.0f)); /** prior on the weight. (lambda = 0.0f is equivalent to a flat prior. )**/
  params_.insert(IntegerParameter("seed", 1234));

// select the optimization and set some generic parameters.
  CompositeParameter optCompositeParam("scg");
  ParameterList& optParams = optCompositeParam.getParams();
  optParams.insert(IntegerParameter("max iterations", 1000));
  optParams.insert(FloatParameter("stopping threshold", 1e-05));
  params_.insert(optCompositeParam);
}

SoftmaxRegression* SoftmaxRegression::clone() const
{
  return new SoftmaxRegression(*this);
}

bool SoftmaxRegression::train(const std::vector<std::vector<float> >& features, const std::vector<uint16_t>& labels)
{
  const uint32_t I = features.size();
  D = features[0].size() + 1;

  if (I == 0 || D == 0) return false;

  std::string optimization = params_.getValue<std::string>("optimization");
  float lambda = 0.0f;

  lambda = params_["lambda"];

  /** determine number of classes from training set **/
  nClasses_ = Math::max(labels) + 1;
  if (nClasses_ < 2) return false;

  // setup the problem.
  L2SoftmaxObjective objective(features, labels, lambda);

  theta_.resize(D * nClasses_, 1);

  Optimization* opt = 0;
  if (optimization == "scg")
  {
#ifdef DEBUG
    std::cout << "using scaled conjugate gradient for optimization" << std::endl;
#endif
    opt = new ScaledCG();
  }
  else
  {
    std::cerr << "unknown optimization '" << optimization << "'! Should be either 'lbfgs', 'scg', or 'gd'."
        << std::endl;
    return false;
  }

  if (params_.hasParam(optimization))
  {
    opt->setParameters(params_[optimization]);
  }

  int32_t retvalue = opt->minimize(objective, theta_, callback_);
  if (retvalue < 0)
  {
    std::cerr << "Warning: Not converged! " << opt->reason(retvalue) << std::endl;
  }

  theta_ = opt->result();

  delete opt;

  trained = true;
  return true; /** we learned a model. **/
}

void SoftmaxRegression::softmax(const Eigen::VectorXd& a, Eigen::VectorXd& s) const
{
  s.resize(a.rows(), a.cols());
  double sum = 0.0;
  double max = a.maxCoeff();

  for (uint32_t i = 0; i < a.size(); ++i)
  {
    s(i, 0) = std::exp(a(i, 0) - max);
    sum += s(i, 0);
  }

  assert(sum > 0.0);

  s /= sum;
}

void SoftmaxRegression::classify(const std::vector<float>& feature, std::vector<float>& conf) const
{
  conf.resize(nClasses_);

  Eigen::VectorXd f(D);
  Eigen::VectorXd s(nClasses_);
  f(0, 0) = bias; /** append bias weight at the beginning. **/

  /* copy feature values. */
  for (uint32_t j = 0; j < feature.size(); ++j)
    f(j + 1, 0) = feature[j];
  //    std::cout << "f = " << f.transpose() << std::endl;

  Eigen::VectorXd a(nClasses_);
  for (uint32_t k = 0; k < nClasses_; ++k)
    a(k, 0) = theta_.segment(k * D, D).transpose() * f; /** phi_k^T*x_i **/

  softmax(a, s); /* ... and calculate softmax. */

  for (uint32_t i = 0; i < nClasses_; ++i)
  {
    assert(!std::isnan(s[i]));
    conf[i] = s[i];
  }
}

bool SoftmaxRegression::save(const std::string& filename, bool overwrite) const
{
  /** check if file exists, and if overwrite is setted. **/
  if (boost::filesystem::exists(filename) && !overwrite) return false;

  std::ofstream out(filename.c_str());
  out << "SoftmaxRegression:0.2" << std::endl;

  out << "<config>" << std::endl;
  for (ParameterList::const_iterator it = params_.begin(); it != params_.end(); ++it)
    out << *it << std::endl;
  out << "</config>" << std::endl;

  out << "weights:" << nClasses_ << " x " << D << std::endl;
  for (uint32_t i = 0; i < nClasses_; ++i)
  {
    if (theta_.cols() > 0) out << theta_(i * D, 0);
    for (uint32_t j = 1; j < D; ++j)
      out << "," << theta_(i * D + j, 0);
    out << std::endl;
  }

  out.close();

  return true;
}

bool SoftmaxRegression::load(const std::string& filename)
{
  params_.clear();
  std::string line;
  std::vector<std::string> tokens;
  std::ifstream in(filename.c_str());
  in.peek();
  std::getline(in, line);
  tokens = split(line, ":");
  if (tokens[0] != "SoftmaxRegression") throw IOError("expected 'SoftmaxRegression', but got '" + tokens[0] + "'");

  std::string version = tokens[1];

  if (version == "0.2")
  {
    std::stringstream paramStream;
    // read parameters.
    do
    {
      std::getline(in, line);
      paramStream << line << std::endl;
      in.peek();
    }
    while (!in.eof() && line != "</config>");

//    std::cout << paramStream.str();
    tinyxml2::XMLDocument doc;
    doc.Parse(paramStream.str().c_str(), paramStream.str().size());
    tinyxml2::XMLElement* rootNode = doc.RootElement();
    tinyxml2::XMLElement* child = rootNode->FirstChildElement();
    for (; child != 0; child = child->NextSiblingElement())
    {
      Parameter* param = Parameter::parseParameter(*child);
      params_.insert(*param);
      delete param;
    }
    // read dimensions
    std::getline(in, line);
    tokens = split(line, ":");
    assert(tokens.size() == 2);
    assert(tokens[0] == "weights");

    tokens = split(tokens[1], "x");
    assert(tokens.size() == 2);
    nClasses_ = boost::lexical_cast<uint32_t>(trim(tokens[0]));
    D = boost::lexical_cast<uint32_t>(trim(tokens[1]));

    // read phi
    theta_ = Eigen::VectorXd::Zero(nClasses_ * D);

    for (uint32_t i = 0; i < nClasses_; ++i)
    {
      std::getline(in, line);
      tokens = split(line, ",");
      assert(tokens.size() == D);

      for (uint32_t j = 0; j < D; ++j)
        theta_(i * D + j, 0) = boost::lexical_cast<double>(tokens[j]);
    }

    in.close();

    trained = true;
  }
  else
  {
    throw IOError("expected version '0.2', but got '" + tokens[1] + "'");
  }

  return true;
}

const Eigen::VectorXd& SoftmaxRegression::getWeights() const
{
  return theta_;
}

}
