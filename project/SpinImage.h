#ifndef SPINIMAGE_H_
#define SPINIMAGE_H_

#include <rv/PointDescriptor.h>
#include <rv/Normalizer.h>

namespace rv
{

/** \brief Spin Image of Johnson and Hebert.
 *
 *  Implementation of the Spin Image for point clouds.
 *
 *  Parameters
 *    num-bins:integer  =  number of bins per dimension.
 *    radius:float      =  max norm radius of points
 *    bilinear:boolean  =  do bilinear interpolation? [default:true]
 *    normalizer:string =  normalizer to use for spin image computation.
 *
 *  By passing an up-vector to the evaluate method, we get an SI in the global reference frame.
 *
 *  \author you
 */
class SpinImage: public PointDescriptor
{
  public:
    SpinImage(const ParameterList& params);
    ~SpinImage();
    SpinImage(const SpinImage& other);
    SpinImage& operator=(const SpinImage& other);

    SpinImage* clone() const;

    void evaluate(float* values, const Point3f& p, const Normal3f& ref, const Laserscan& scan,
        const NearestNeighborImpl& nn) const;
    uint32_t dim() const;

  protected:
    uint32_t num_bins_;
    float radius_;
    bool bilinear_interp_;

    rv::MaximumNorm norm_;
    rv::Normalizer* normalizer_;
};

}

#endif /* SPINIMAGE_H_ */
