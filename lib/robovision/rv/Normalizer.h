#ifndef RV2_NORMALIZER_H_
#define RV2_NORMALIZER_H_

// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include <vector>
#include <string>
#include <stdint.h>

namespace rv
{

/** \brief normalization of feature vectors.
 *
 *  \author behley
 */
class Normalizer
{
  public:
    Normalizer();
    virtual ~Normalizer();

    virtual Normalizer* clone() const = 0;

    /** \brief normalize given feature vector **/
    virtual void normalize(float* feature, uint32_t D) const = 0;

    /** \brief learn normalizer from data. **/
    virtual void train(const std::vector<std::vector<float> >& features);

    virtual bool save(const std::string& filename, bool overwrite = false) const;
    virtual bool load(const std::string& filename);

    /** \brief to avoid dividing by zero, we may want add epsilon to normalizer. Default value is 1e-6.**/
    void setEpsilon(float eps);

  protected:
    float epsilon_;
};

/** some convenience functions to load a specific normalizer. **/
Normalizer* getNormalizerByName(const std::string& name);

class NullNormalizer: public Normalizer
{
  public:
    NullNormalizer* clone() const
    {
      return new NullNormalizer;
    }

    void normalize(float* feature, uint32_t D) const
    {
      // NOOP.
    }
};

/**
 * \brief normalize by L1 norm of feature vector.
 */
class L1Normalizer: public Normalizer
{
  public:
    L1Normalizer* clone() const
    {
      return new L1Normalizer;
    }

    void normalize(float* feature, uint32_t D) const;
};

/**
 * \brief normalize by L2 norm of feature vector.
 */
class L2Normalizer: public Normalizer
{
  public:
    L2Normalizer* clone() const
    {
      return new L2Normalizer;
    }

    void normalize(float* feature, uint32_t D) const;
};

/**
 * \brief normalize by maximum norm of feature vector.
 */
class MaxNormalizer: public Normalizer
{
  public:
    MaxNormalizer* clone() const
    {
      return new MaxNormalizer;
    }

    void normalize(float* feature, uint32_t D) const;
};

/**
 * \brief normalize [x_1, ..., x_N] by dividing every x_i entry by learned values eta_i.
 */
class MaxEntryNormalizer: public Normalizer
{
  public:
    MaxEntryNormalizer* clone() const
    {
      return new MaxEntryNormalizer(*this);
    }

    void normalize(float* feature, uint32_t D) const;

    void train(const std::vector<std::vector<float> >& features);

    void setEta(const std::vector<float>& eta);
    const std::vector<float>& getEta() const;

    bool save(const std::string& filename, bool overwrite = false) const;
    bool load(const std::string& filename);

  protected:
    std::vector<float> eta_;
};

/** TODO: \brief normalize by scaling features. **/
//class Standardizer: public Normalizer
//{
//  public:
//    void normalize(std::vector<float>& feature) const;
//
//    void train(const std::vector<std::vector<float> >& features);
//};
/** TODO: \brief normalize by rotating and whitening of feature vectors. **/
//class ZCANormalization: public Normalizer
//{
//  public:
//    void normalize(std::vector<float>& feature) const;
//
//    void train(const std::vector<std::vector<float> >& features);
//};
/** TODO: \brief normalize by rotating and whitening of feature vectors. **/
//class Whitening: public Normalizer
//{
//  public:
//    void normalize(std::vector<float>& feature) const;
//
//    void train(const std::vector<std::vector<float> >& features);
//};
} /* namespace rv */
#endif /* NORMALIZER_H_ */
