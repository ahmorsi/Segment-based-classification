// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef RVCOORDINATESYSTEM_H_
#define RVCOORDINATESYSTEM_H_

#include "Transform.h"

/** \brief some common transformations **/

namespace rv
{
/** \brief coordinate transform from RoSe to OpenGL coordinates. **/
Transform RoSe2GL = Transform(0, -1, 0, 0,
               0,  0, 1, 0,
              -1,  0, 0, 0,
               0,  0, 0, 1);

/** \brief coordinate transform from RoSe to OpenGL coordinates. **/
Transform GL2RoSe = Transform(0, 0,-1, 0,
              -1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 1);
}

#endif /* COORDINATESYSTEM_H_ */
