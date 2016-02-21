// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.
//

#ifndef ERROR_H_
#define ERROR_H_

#include <exception>
#include <string>

/** \brief 
 *  
 *
 * 
 *  \author behley
 */

namespace rv
{
class Error: public std::exception
{
  public:
    Error(const std::string& reason) :
        reason_(reason)
    {
    }
    virtual ~Error() throw ()
    {
    }

    const char* what() const throw ()
    {
      return reason_.c_str();
    }
  protected:
    std::string reason_;
};
}

#endif /* ERROR_H_ */
