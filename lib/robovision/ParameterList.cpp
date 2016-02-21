// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/ParameterList.h"
#include "rv/tinyxml2.h"
#include "rv/IOError.h"

namespace rv
{

ParameterList::ParameterList()
{

}

ParameterList::~ParameterList()
{
  for (std::map<std::string, Parameter*>::const_iterator it = params.begin(); it != params.end(); ++it)
    delete (it->second);
  params.clear();
}

ParameterList::ParameterList(const ParameterList& other)
{
  for (std::map<std::string, Parameter*>::const_iterator it = other.params.begin(); it != other.params.end(); ++it)
    params[it->first] = it->second->clone();
}

ParameterList& ParameterList::operator=(const ParameterList& other)
{
  if (&other == this) return *this;
  // remove old stuff.
  for (std::map<std::string, Parameter*>::const_iterator it = params.begin(); it != params.end(); ++it)
    delete it->second;
  params.clear();
  // copy new stuff!
  for (std::map<std::string, Parameter*>::const_iterator it = other.params.begin(); it != other.params.end(); ++it)
    params[it->first] = it->second->clone();

  return *this;
}

bool ParameterList::operator==(const ParameterList& other) const
{
  for (std::map<std::string, Parameter*>::const_iterator it = other.params.begin(); it != other.params.end(); ++it)
  {
    const Parameter& param1 = *(it->second);
    bool found = false;
    for (std::map<std::string, Parameter*>::const_iterator it2 = params.begin(); it2 != params.end(); ++it2)
      if (param1 == *(it2->second) && param1.name() == it2->second->name()) found = true;

    if (!found) return false;
  }

  return true;
}

bool ParameterList::operator!=(const ParameterList& other) const
{
  return !(*this == other);
}

void ParameterList::insert(const Parameter& param)
{
  std::map<std::string, Parameter*>::iterator el = params.find(param.name());
  if (el != params.end())
  {
    // delete old parameter.
    delete el->second;
    params.erase(el);
  }
  params[param.name()] = param.clone();
}

//void ParameterList::insert(Parameter* param)
//{
//  if (param == 0) return;
//  std::map<std::string, Parameter*>::iterator el = params.find(param->name());
//  if (el != params.end())
//  {
//    // delete old parameter.
//    delete el->second;
//    params.erase(el);
//  }
//  /** no clone! **/
//  params[param->name()] = param;
//}

void ParameterList::erase(const std::string& name)
{
  std::map<std::string, Parameter*>::iterator el = params.find(name);
  if (el != params.end())
  {
    delete el->second;
    params.erase(el);
  }
}

bool ParameterList::hasParam(const std::string& name) const
{
  return (params.find(name) != params.end());
}

ParameterList::const_iterator ParameterList::begin() const
{
  return ParameterList::const_iterator(params.begin());
}

ParameterList::const_iterator ParameterList::end() const
{
  return ParameterList::const_iterator(params.end());
}

void ParameterList::checkParam(const std::string& name) const
{
  if (params.find(name) != params.end()) return;

  std::stringstream str;
  str << "no parameter with name " << name << " in parameter list";
  throw Error(str.str());
}

void ParameterList::clear()
{
  for (std::map<std::string, Parameter*>::iterator it = params.begin(); it != params.end(); ++it)
    delete it->second;
  params.clear();
}

void parseXmlFile(const std::string& filename, ParameterList& params)
{
  using namespace tinyxml2;

  XMLDocument document;
  document.LoadFile(filename.c_str());
  if (document.Error())
  {
    document.PrintError();
    std::stringstream serr;
    serr << "Error while parsing XML (Err: " << document.ErrorName() << ": " << document.GetErrorStr1() << ", "
        << document.GetErrorStr2() << ")";
    throw Error(serr.str());
  }

  tinyxml2::XMLElement* root = document.RootElement();
  if (root == 0) throw IOError("No root element.");

  for (tinyxml2::XMLElement* child = root->FirstChildElement("param"); child != 0;
      child = child->NextSiblingElement("param"))
  {
    Parameter* param = Parameter::parseParameter(*child);
    params.insert(*param);
    delete param;
  }
}

}
