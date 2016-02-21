#include "SpinImage.h"
#include <rv/Math.h>

using namespace rv;

SpinImage::SpinImage(const ParameterList& params) :
    PointDescriptor(params), normalizer_(0)
{
  num_bins_ = params["num-bins"];
  radius_ = params["radius"];
  bilinear_interp_ = true;
  normalizer_ = new MaxNormalizer();

  if (params.hasParam("bilinear")) bilinear_interp_ = params["bilinear"];
  if (params.hasParam("normalizer"))
  {
    delete normalizer_;
    normalizer_ = getNormalizerByName(params["normalizer"]);
  }
}

SpinImage::SpinImage(const SpinImage& other) :
    PointDescriptor(other), num_bins_(other.num_bins_), radius_(other.radius_), bilinear_interp_(
        other.bilinear_interp_), normalizer_(other.normalizer_->clone())
{

}

SpinImage& SpinImage::operator=(const SpinImage& other)
{
  params_ = other.params_;
  num_bins_ = other.num_bins_;
  radius_ = other.radius_;
  bilinear_interp_ = other.bilinear_interp_;

  delete normalizer_;
  normalizer_ = other.normalizer_->clone();

  return *this;
}

SpinImage::~SpinImage()
{
  delete normalizer_;
}

SpinImage* SpinImage::clone() const
{
  return new SpinImage(*this);
}

void SpinImage::evaluate(float* values, const Point3f& p, const Normal3f& ref, const Laserscan& scan,
    const NearestNeighborImpl& nn) const
{
  /** initialize values with zeros **/
  memset(values, 0, dim() * sizeof(float));
  std::vector<uint32_t> neighbors;
  // TODO: Implement computation of spin images and optionally bi-linear interpolation.
  MaximumNorm norm;
  nn.radiusNeighbors(p,radius_,neighbors,norm);
  float cellSize = radius_*1/num_bins_;
  for(int idx=0;idx<neighbors.size();++idx)
  {
	  const Point3f& q = scan.point(neighbors[idx]);
	  Vector3f LinePointDistVect = q-p;
	  Eigen::Vector3f r,LP_Dist;
	  LP_Dist << LinePointDistVect.x(),LinePointDistVect.y(),LinePointDistVect.z();
	  r << ref.x(),ref.y(),ref.z();
	  Eigen::Vector3f A = r.cross(LP_Dist);
	  float alpha = A.norm();
	  float beta = r.transpose()*LP_Dist;

	  int i= (int)floor(alpha/cellSize);
	  int j = (int)floor((beta + radius_)/(2*cellSize));
	  if(i >= num_bins_  || j >= num_bins_)
		  continue;
	  ++ values[j*num_bins_ + i];
	  if(bilinear_interp_)
	   {
	 	  int a = alpha-i*cellSize;
	 	  int b = ((beta+radius_)/2)-i*cellSize;

	 	values[j*num_bins_ + i] += (1-a)*(1-b);
	 	values[j*num_bins_ + i + 1] += a*(1-b);
	 	values[(j+1)*num_bins_ + i] += (1-a)*b;
	 	values[(j+1)*num_bins_ + i + 1] += a*b;
	   }
  }

  normalizer_->normalize(values, num_bins_ * num_bins_);
}

uint32_t SpinImage::dim() const
{
  return num_bins_ * num_bins_;
}
