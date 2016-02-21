// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/PrimitiveParameters.h"
#include <sstream>
#include <cstdlib>
#include <cstdio>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace rv
{
/******************************************************************************
 *                             BooleanParameter                               *
 ******************************************************************************/

BooleanParameter::BooleanParameter(const std::string& name, const bool value) :
    Parameter(name), val(value)
{
}

BooleanParameter* BooleanParameter::clone() const
{
  return new BooleanParameter(*this);
}

std::string BooleanParameter::toString() const
{
  std::stringstream out;
  out << "<param name=\"" << mName << "\" type=\"boolean\">" << (val ? "true" : "false") << "</param>";
  return out.str();
}

BooleanParameter* parseBooleanParameter(const tinyxml2::XMLElement& node)
{
  if (node.Attribute("type", "boolean"))
  {
    std::string text(node.GetText());
    return new BooleanParameter(node.Attribute("name"), (text == "true"));
  }
  else
    throw Error("Error while parameter parsing: 'type' not 'boolean'");
}

/******************************************************************************
 *                             IntegerParameter                               *
 ******************************************************************************/

IntegerParameter::IntegerParameter(const std::string& name, const int value) :
    Parameter(name), val(value)
{
}

IntegerParameter* IntegerParameter::clone() const
{
  return new IntegerParameter(*this);
}

std::string IntegerParameter::toString() const
{
  std::stringstream out;
  out << "<param name=\"" << mName << "\" type=\"integer\">" << val << "</param>";
  return out.str();
}

IntegerParameter* parseIntegerParameter(const tinyxml2::XMLElement& node)
{
  if (node.Attribute("type", "integer"))
  {
    std::string text(node.GetText());
    boost::algorithm::trim(text);
    int32_t value = boost::lexical_cast<int32_t>(text);
    return new IntegerParameter(node.Attribute("name"), value);
  }
  else
    throw Error("Error while parameter parsing: 'type' not 'integer'");
}

/******************************************************************************
 *                             FloatParameter                                 *
 ******************************************************************************/

FloatParameter::FloatParameter(const std::string& name, const double value) :
    Parameter(name), val(value)
{
}

FloatParameter* FloatParameter::clone() const
{
  return new FloatParameter(*this);
}

std::string FloatParameter::toString() const
{
  std::stringstream out;
  out << "<param name=\"" << mName << "\" type=\"float\">" << val << "</param>";
  return out.str();
}

FloatParameter* parseFloatParameter(const tinyxml2::XMLElement& node)
{
  if (node.Attribute("type", "float"))
  {
    std::string text(node.GetText());
    boost::algorithm::trim(text);
    float value = boost::lexical_cast<float>(text);
    return new FloatParameter(node.Attribute("name"), value);
  }
  else
    throw Error("Error while parameter parsing: 'type' not 'float'");
}

/******************************************************************************
 *                             StringParameter                                *
 ******************************************************************************/

StringParameter::StringParameter(const std::string& name, const std::string value) :
    Parameter(name), val(value)
{
}

StringParameter* StringParameter::clone() const
{
  return new StringParameter(*this);
}

std::string StringParameter::toString() const
{
  std::stringstream out;

  out << "<param name=\"" << mName << "\" type=\"string\">" << val << "</param>";

  return out.str();
}

StringParameter* parseStringParameter(const tinyxml2::XMLElement& node)
{
  if (node.Attribute("type", "string"))
    return new StringParameter(node.Attribute("name"), node.GetText());
  else
    throw Error("Error while parameter parsing: 'type' not 'string'");
}

}
