// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef NEARESTNEIGHBORIMPL_H_
#define NEARESTNEIGHBORIMPL_H_

#include <vector>
#include "geometry.h"
#include "norms.h"


namespace rv
{

/** \brief interface for a neighbor search implementation
 * 
 *  \author behley
 */
class NearestNeighborImpl
{
  public:
    virtual ~NearestNeighborImpl();

    /** \brief initialize nearest neighbor data structure with given points **/
    void initialize(const std::vector<Point3f>& pts);

    /** \brief initialize nearest neighbor data structure with subset of points specified by indexes **/
    virtual void initialize(const std::vector<Point3f>& pts, const std::vector<uint32_t>& indexes) = 0;

    /** \brief search radius neighbors = {p | ||p-q|| < r }
     *
     * @param q         query point q
     * @param r         maximal distance of neighbors
     * @param neighbors contains indexes of neighbor points
     * @param dist      norm for radius neighbors search
     */
    virtual void radiusNeighbors(const Point3f& q, float r, std::vector<uint32_t>& neighbors, const Norm& dist) const = 0;
};

}

#endif /* NEARESTNEIGHBORIMPL_H_ */
