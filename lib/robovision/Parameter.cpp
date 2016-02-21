// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Parameter.h"
#include "rv/PrimitiveParameters.h"
#include "rv/CompositeParameter.h"
//#include "rv/VectorParameter.h"
//#include "rv/MatrixParameter.h"
//#include "rv/RangeParameter.h"

#include "rv/ParameterList.h"

namespace rv
{

Parameter::operator ParameterList() const
{
  std::stringstream str;
  str << "conversion of Parameter with name '" << mName << "' to ParameterList not possible.";
  throw Error(str.str());
}

Parameter* Parameter::parseParameter(const tinyxml2::XMLElement& node)
{
  const std::string param_name = node.Attribute("name");
  const std::string param_type = node.Attribute("type");

  if (param_type == "integer")
    return parseIntegerParameter(node);
  else if (param_type == "float")
    return parseFloatParameter(node);
  else if (param_type == "string")
    return parseStringParameter(node);
  else if (param_type == "boolean")
    return parseBooleanParameter(node);
//  else if (param_type == "vector")
//    return parseVectorParameter(node);
//  else if (param_type == "matrix")
//    return parseMatrixParameter(node);
//  else if (param_type == "range")
//    return parseRangeParameter(node);
  else if (param_type == "composite")
    return parseCompositeParameter(node);
  else
  {
    std::stringstream reason;
    reason << "Unknown parameter type '" << param_type << "' in xml file " << std::endl;
    throw Error(reason.str());
  }

}

}
