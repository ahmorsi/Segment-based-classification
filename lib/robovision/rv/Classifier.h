// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <stdint.h>

#include "ParameterList.h"
#include "OptimizationCallback.h"

namespace rv
{

/** \brief base-class for classifiers.
 *
 *  \author behley
 */
class Classifier
{
  public:
    Classifier();
    Classifier(const Classifier& other);
    virtual ~Classifier();

    Classifier& operator=(const Classifier& other);

    /** \brief generate a copy of a Classifier instance.
     *
     *  As the type of a Classifier* instance is unknown, we have to
     *  copy a Classifier* using a clone() method. The method implemented
     *  by the sub-classes guarantees the proper copying of all member
     *  variables.
     **/
    virtual Classifier* clone() const = 0;

    /** \brief learn the classifier directly from the feature values and labels
     *
     *  this method assumes there are K classes with labels in [0, ..., K-1]
     *
     *  \return true, if training was successful, false otherwise.
     */
    virtual bool
    train(const std::vector<std::vector<float> >& features, const std::vector<uint16_t>& labels) = 0;

    /**
     * \brief classifies the given a feature vector and returns the probability p(y|x).
     */
    virtual void
    classify(const std::vector<float>& feature, std::vector<float>& prob) const = 0;

    /** \brief set the parameters of the classifier **/
    void setParameters(const ParameterList& list);

    /** \brief returns the parameters. **/
    const ParameterList& getParameters() const;

    /** \brief storing a classification model.
     *  \param overwrite if true, any existing file will be overwritten.
     *
     *  \return true, if model was written, false otherwise.
     **/
    virtual bool
    save(const std::string& filename, bool overwrite = false) const = 0;

    /** \brief loading a classification model.
     *  \return true, if model was written, false otherwise.
     **/
    virtual bool load(const std::string& filename) = 0;

    /** \brief setting the callback to monitor the optimization progress in training. **/
    virtual void setCallback(OptimizationCallback* callback);

    uint32_t numClasses() const;

  protected:
    ParameterList params_; /** parameters of the classifier **/
    uint32_t nClasses_; /** number of classes **/

    OptimizationCallback* callback_;
};

}

#endif /* CLASSIFIER_H_ */
