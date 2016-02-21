#ifndef BAGOFWORDSDESCRIPTOR_H_
#define BAGOFWORDSDESCRIPTOR_H_

#include <rv/SegmentDescriptor.h>
#include <rv/PointDescriptor.h>
#include <rv/ParameterList.h>
#include <rv/Normalizer.h>

/** \brief Implementation of a Bag-of-Words descriptor for a segment
 *
 *  The descriptor takes a pre-trained vocabulary and a point descriptor
 *  to compute a Bag-of-Words histogram.
 * 
 *  \author behley
 */

class BagOfWordsDescriptor: public rv::SegmentDescriptor
{
  public:
    BagOfWordsDescriptor(const rv::ParameterList& params, const rv::PointDescriptor& descriptor,
        const std::vector<std::vector<float> >& vocabulary);
    ~BagOfWordsDescriptor();
    BagOfWordsDescriptor(const BagOfWordsDescriptor& other);
    BagOfWordsDescriptor& operator=(const BagOfWordsDescriptor& other);

    BagOfWordsDescriptor* clone() const;

    void evaluate(float* values, const rv::IndexedSegment& segment, const rv::Laserscan& scan,
        const rv::NearestNeighborImpl& nn) const;

    uint32_t dim() const;

  protected:
    float distance(const std::vector<float>& a, const std::vector<float>& b) const;

    rv::PointDescriptor* descriptor_;
    rv::Normalizer* normalizer_;
    std::vector<std::vector<float> > vocabulary_;
};

#endif /* BAGOFWORDSDESCRIPTOR_H_ */
