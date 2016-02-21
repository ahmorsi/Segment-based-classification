// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Normalizer.h"
#include <exception>
#include <cmath>
#include <stdint.h>
#include <cassert>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <rv/string_utils.h>

namespace rv
{

Normalizer::Normalizer() :
    epsilon_(0.000001f)
{
}

Normalizer::~Normalizer()
{

}

/** \brief learn normalizer from data. **/
void Normalizer::train(const std::vector<std::vector<float> >&)
{

}

bool Normalizer::save(const std::string&, bool) const
{
  return true;
}

bool Normalizer::load(const std::string&)
{
  return true;
}

/** some convenience functions to load a specific normalizer. **/
Normalizer* getNormalizerByName(const std::string& name)
{
  if (name == "L1" || name == "local-sum")
    return new L1Normalizer();
  else if (name == "L2")
    return new L2Normalizer();
  else if (name == "max" || name == "local-max")
    return new MaxNormalizer();
  else if (name == "entry-max") return new MaxEntryNormalizer();

  return new NullNormalizer();
}

/**
 * \brief normalize by L1 norm of feature vector.
 */
void L1Normalizer::normalize(float* feature, uint32_t D) const
{
  float eta = epsilon_;
  for (uint32_t j = 0; j < D; ++j)
    eta += feature[j];

  assert(eta > 0.0f);

  for (uint32_t j = 0; j < D; ++j)
    feature[j] /= eta;
}

void L2Normalizer::normalize(float* feature, uint32_t D) const
{
  float eta = epsilon_;
  for (uint32_t j = 0; j < D; ++j)
    eta += feature[j] * feature[j];

  assert(eta > 0.0f);
  eta = std::sqrt(eta);

  for (uint32_t j = 0; j < D; ++j)
    feature[j] /= eta;
}

void MaxNormalizer::normalize(float* feature, uint32_t D) const
{
  float eta = epsilon_;
  for (uint32_t j = 0; j < D; ++j)
    eta = std::max(feature[j], eta);

  assert(eta > 0.0f);

  for (uint32_t j = 0; j < D; ++j)
    feature[j] /= eta;
}

void MaxEntryNormalizer::normalize(float* feature, uint32_t D) const
{
  assert(D == eta_.size());
  for (uint32_t i = 0; i < D; ++i)
  {
    feature[i] /= eta_[i];
  }
}

void MaxEntryNormalizer::train(const std::vector<std::vector<float> >& features)
{
  assert(features.size() > 0);
  assert(features[0].size() > 0);
  const uint32_t N = features.size();
  const uint32_t D = features[0].size();
  eta_.resize(D);
  for (uint32_t i = 0; i < D; ++i)
    eta_[i] = features[0][i];

  for (uint32_t j = 0; j < N; ++j)
  {
    const std::vector<float>& feature = features[j];
    for (uint32_t i = 0; i < D; ++i)
    {
      eta_[i] = std::max(eta_[i], feature[i]);
    }
  }
}

void MaxEntryNormalizer::setEta(const std::vector<float>& eta)
{
  eta_ = eta;
}

const std::vector<float>& MaxEntryNormalizer::getEta() const
{
  return eta_;
}

bool MaxEntryNormalizer::save(const std::string& filename, bool overwrite) const
{
  if (boost::filesystem::exists(filename) && !overwrite) return false;

  std::ofstream out(filename.c_str());
  if (!out.is_open()) return false;

  out << "MaxEntryNormalizer:1.0" << std::endl;
  out << "eta:";
  for (uint32_t i = 0; i < eta_.size(); ++i)
  {
    if (i > 0) out << ",";
    out << eta_[i];
  }
  out << std::endl;

  out.close();

  return true;
}

bool MaxEntryNormalizer::load(const std::string& filename)
{
  std::string line;
  std::vector<std::string> tokens;
  std::ifstream in(filename.c_str());
  if (!in.is_open()) return false;

  std::getline(in, line);
  tokens = split(line, ":");

  if (tokens.size() == 1)
  {
    // assuming old version of the segment_normalizer file...
    tokens = split(line, ",");

    if (tokens.size() == 0) return false;
    eta_.clear();
    for (uint32_t i = 0; i < tokens.size(); ++i)
      eta_.push_back(boost::lexical_cast<float>(tokens[i]));

    return true;
  }

  if (tokens.size() != 2 || tokens[0] != "MaxEntryNormalizer") return false;

  std::string version = tokens[1];
  if (version == "1.0")
  {
    std::getline(in, line);
    tokens = split(line, ":");
    if (tokens.size() != 2 || tokens[0] != "eta") return false;
    tokens = split(tokens[1], ",");

    eta_.clear();
    for (uint32_t i = 0; i < tokens.size(); ++i)
      eta_.push_back(boost::lexical_cast<float>(tokens[i]));
  }

  in.close();

  return true;
}

}
