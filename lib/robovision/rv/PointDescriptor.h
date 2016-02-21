// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef POINTDESCRIPTOR_H_
#define POINTDESCRIPTOR_H_

#include <stdint.h>
#include <vector>

#include "ParameterList.h"
#include "Laserscan.h"
#include "NearestNeighborImpl.h"

namespace rv
{

/**
 * \brief Point-based PointDescriptor for three-dimensional point clouds.
 *
 * Base class for a PointDescriptor which provides a method to compute a feature vector for a given point.
 *
 * \author behley
 ***/
class PointDescriptor
{
  public:
    /** \brief initialize PointDescriptor. **/
    PointDescriptor();

    /** \brief initialize PointDescriptor using a parameter list **/
    PointDescriptor(const ParameterList& params);
    virtual ~PointDescriptor();

    virtual PointDescriptor* clone() const = 0;

    /** \brief evaluate the PointDescriptor and write results to values.
     *
     *  caller has to ensure that there is enough space in values.
     *
     *  \param values   pointer to start of feature values.
     *  \param p        query point
     *  \param ref      normal/reference axis of query point
     *  \param scan     points used to generate the nearest neighbor datastructure
     *  \param nn       nearest neighbor implementation using pts.
     *
     */
    virtual void evaluate(float* values, const Point3f& p, const Normal3f& ref, const Laserscan& scan,
        const NearestNeighborImpl& nn) const = 0;

    /** \brief dimension of the PointDescriptor. **/
    virtual uint32_t dim() const = 0;

    /** \brief parameter of the PointDescriptor. **/
    const ParameterList& params() const;

  protected:
    ParameterList params_;
};

}
#endif /* POINTDESCRIPTOR_H_ */
