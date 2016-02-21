// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef SEGMENTDESCRIPTOR_H_
#define SEGMENTDESCRIPTOR_H_

#include "ParameterList.h"
#include "Laserscan.h"
#include "IndexedSegment.h"
#include "NearestNeighborImpl.h"


namespace rv
{

/** \brief Interface for a segment descriptor
 *  
 *  \author behley
 */
class SegmentDescriptor
{
  public:
    SegmentDescriptor(const ParameterList& params);
    virtual ~SegmentDescriptor();

    virtual SegmentDescriptor* clone() const = 0;

    virtual void evaluate(float* values, const IndexedSegment& segment, const Laserscan& scan,
        const NearestNeighborImpl& nn) const = 0;

    /** \brief dimension of the SegmentDescriptor. **/
    virtual uint32_t dim() const = 0;

    /** \brief parameters of the SegmentDescriptor. **/
    const ParameterList& params() const;

  protected:
    ParameterList params_;
};

} /* namespace rv */
#endif /* SEGMENTDESCRIPTOR_H_ */
