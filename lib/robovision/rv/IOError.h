// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef IOERROR_H_
#define IOERROR_H_

#include "Error.h"

/** \brief Exception for input/output errors while reading/writing streams, files, etc.
 * 
 *  \author behley
 */

namespace rv
{

class IOError: public Error
{
  public:
    IOError(const std::string& reason) :
        Error(reason)
    {
    }
    virtual ~IOError() throw ()
    {
    }

};

}
#endif /* IOERROR_H_ */
