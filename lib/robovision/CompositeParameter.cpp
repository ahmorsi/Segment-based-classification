// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/CompositeParameter.h"
#include <sstream>

namespace rv
{

CompositeParameter::CompositeParameter(const std::string& name) :
    Parameter(name)
{

}

CompositeParameter::CompositeParameter(const std::string& name, const ParameterList& values) :
    Parameter(name), param_list(values)
{

}

CompositeParameter::CompositeParameter(const std::string& name, const tinyxml2::XMLElement& node) :
    Parameter(name)
{
  const tinyxml2::XMLElement* child;

  for (child = node.FirstChildElement("param"); child != 0; child = child->NextSiblingElement("param"))
  {
    Parameter* p = parseParameter(*child);
    if (param_list.hasParam(p->name())) std::cout << "WARNING: (" << name << ") parameter with name = '" << p->name()
        << "' already inserted." << std::endl;
    param_list.insert(*p);
    delete p;
  }
}

CompositeParameter::~CompositeParameter()
{

}

CompositeParameter::CompositeParameter(const CompositeParameter& other) :
    Parameter(other), param_list(other.param_list)
{

}

CompositeParameter& CompositeParameter::operator=(const CompositeParameter& other)
{
  if (&other == this) return *this;

  Parameter::operator=(other);

  param_list = other.param_list;

  return *this;
}

CompositeParameter* CompositeParameter::clone() const
{
  return new CompositeParameter(*this);
}

std::string CompositeParameter::toString() const
{
  std::stringstream out;

  out << "<param name=\"" << mName << "\" type=\"composite\">" << std::endl;
  for (ParameterList::const_iterator it = param_list.begin(); it != param_list.end(); ++it)
    out << "   " << *it << std::endl;
  out << "</param>";

  return out.str();
}

CompositeParameter::operator ParameterList() const
{
  return param_list;
}

CompositeParameter* parseCompositeParameter(const tinyxml2::XMLElement& node)
{
  return new CompositeParameter(node.Attribute("name"), node);
}

}
