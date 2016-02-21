#ifndef L2SOFTMAXOBJECTIVE_H_
#define L2SOFTMAXOBJECTIVE_H_

#include <rv/Objective.h>
#include <vector>
#include <stdint.h>

/** \brief objective for softmax regression with L2 regularization
 *
 */
class L2SoftmaxObjective: public rv::Objective
{
  public:
    /** \brief initialize objective with features X \in N x M (without bias), y \in \[0,...,K-1\], and lambda
     *
     *  The feature vectors get implicitly added a bias term and the labels are expected to have a value in [0:K-1].
     **/
    L2SoftmaxObjective(const std::vector<std::vector<float> >& features,
        const std::vector<uint16_t>& labels, float _lambda = 0.0f);

    double operator()(const Eigen::VectorXd& x);
    double operator()(const Eigen::VectorXd& x, Eigen::VectorXd& grad);

  protected:
    /** \brief dot product of [1,x] and k-th part of theta, i.e. theta(k*M:(k+1)*M-1) **/
    double kDot(const std::vector<float>& x, const Eigen::VectorXd& theta,
        uint32_t k);
    Eigen::VectorXd CalculateGradient(uint16_t j,const Eigen::VectorXd& theta);
    const std::vector<std::vector<float> >& X_;
    const std::vector<uint16_t>& Y_;
    float lambda_;

    uint32_t K_; // number of classes
    uint32_t N_; // number of instances
    uint32_t D_; // feature dimension + bias term.
};


#endif /* L2SOFTMAXOBJECTIVE_H_ */
