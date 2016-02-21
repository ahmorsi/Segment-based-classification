// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef LASERSCAN_H_
#define LASERSCAN_H_

#include <stdint.h>
#include <vector>

#include "geometry.h"
#include "Transform.h"

namespace rv
{

/** \brief Representation of a laser range scan with remissions and normals.
 *  
 *  \author behley
 */

class Laserscan
{
  public:
    /** \brief empty laser range scan. **/
    Laserscan();

    /** \brief remove data. **/
    void clear();

    /** \brief getter for points/normals/remission **/
    const Point3f& point(uint32_t i) const;
    float remission(uint32_t i) const;
    const Normal3f& normal(uint32_t) const;

    /** \brief global pose of the sensor. **/
    Transform& pose();
    const Transform& pose() const;
    /** \brief number of points. **/
    uint32_t size() const;

    /** access to the raw data. **/
    std::vector<Point3f>& points();
    const std::vector<Point3f>& points() const;
    std::vector<float>& remissions();
    const std::vector<float>& remissions() const;
    std::vector<Normal3f>& normals();
    const std::vector<Normal3f>& normals() const;

    bool hasRemission() const;
    bool hasNormals() const;

  protected:
    Transform pose_;
    /** representing points, normals, and remission this way, we can separately initialize points and normals. **/
    std::vector<Point3f> points_;
    std::vector<float> remissions_;
    std::vector<Normal3f> normals_;
};

}

#endif /* LASERSCAN_H_ */
