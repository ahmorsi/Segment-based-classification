// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef SOFTMAXREGRESSION_H_
#define SOFTMAXREGRESSION_H_

#include <rv/Classifier.h>
#include <eigen3/Eigen/Dense>

// forward declaration of test case.
class SoftmaxRegressionTestCase;

namespace rv
{

/**
 * \brief straight-forward implementation of a multi-class logistic regression.
 *
 *  The multi-class case is handled by a softmax function.
 *  This implementation uses SCG or GD to maximize the log-likelihood [1].
 *
 *  [1] Simon J. D. Prince. Computer Vision: Models, Learning and Inference. Cambridge University Press, 2012.
 *
 *  see also L2SoftmaxObjective.h
 *
 *  \author jens behley
 */
class SoftmaxRegression: public Classifier
{
  public:
    friend class ::SoftmaxRegressionTestCase;
    SoftmaxRegression();
    SoftmaxRegression* clone() const;

    /** \brief learn the classifier directly from the feature values and labels \in \{0, ..., K-1\}
     *
     *  \return true, if training was successful, false otherwise.
     */
    bool train(const std::vector<std::vector<float> >& features, const std::vector<uint16_t>& labels);

    /** \brief classify the given data points using the previously learned weights.
     *
     *  \return Probabilities P(y=l|x) for l \in [0,..., K-1].
     **/
    void classify(const std::vector<float>& feature, std::vector<float>& conf) const;

    bool save(const std::string& filename, bool overwrite = false) const;
    bool load(const std::string& filename);

    const Eigen::VectorXd& getWeights() const;
    void setWeights(const Eigen::VectorXd& weights);

  protected:
    /** \brief returns the softmax values for activations in a **/
    void softmax(const Eigen::VectorXd& a, Eigen::VectorXd& l) const;

    Eigen::VectorXd theta_; /** parameters of the fitted model. \in R^{k*D \times 1} **/
    float bias;
    uint32_t D; /** dimension of the feature vector. (includes bias weight) **/

    bool trained;
};

}
#endif /* SOFTMAXREGRESSION_H_ */
