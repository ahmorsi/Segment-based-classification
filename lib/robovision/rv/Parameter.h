// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <string>
#include <iostream>
#include <map>
#include <sstream>

#include "tinyxml2.h"
#include "Error.h"


namespace rv
{

class ParameterList;

/**
 * \brief abstract base class for parameters
 *
 * Using the method parseParameter a RoSe::XmlNode can be parsed into the
 * corresponding parameter type. Parameters should implement the conversion
 * operations, if the conversion is possible.
 *
 * \author behley
 */
class Parameter
{
  public:
    typedef std::map<std::string, Parameter*> ParameterMap;

    virtual ~Parameter()
    {

    }

    virtual operator bool() const
    {
      std::stringstream str;
      str << "conversion of Parameter to bool with name '" << mName
          << "' not possible.";
      throw Error(str.str());
    }

    virtual operator int() const
    {
      std::stringstream str;
      str << "conversion of Parameter to int with name '" << mName
          << "' not possible.";
      throw Error(str.str());
    }

    virtual operator unsigned int() const
    {
      std::stringstream str;
      str << "conversion of Parameter  with name '" << mName
          << "' to unsigned int not possible.";
      throw Error(str.str());
    }

    virtual operator double() const
    {
      std::stringstream str;
      str << "conversion of Parameter  with name '" << mName
          << "' to double not possible.";
      throw Error(str.str());
    }

    virtual operator float() const
    {
      std::stringstream str;
      str << "conversion of Parameter to float with name '" << mName
          << "' to double not possible.";
      throw Error(str.str());
    }

    virtual operator std::string() const
    {
      std::stringstream str;
      str << "conversion of Parameter to std::string with name '" << mName
          << "' not possible.";
      throw Error(str.str());
    }

    virtual operator ParameterList() const;

    virtual Parameter* clone() const = 0;

    virtual std::string toString() const = 0;
    virtual std::string valueStr() const
    {
      return ""; /** empty string if not appropriately implemented. **/
    }

    std::string& name()
    {
      return mName;
    }

    std::string name() const
    {
      return mName;
    }

    static Parameter* parseParameter(const tinyxml2::XMLElement& node);

    friend std::ostream& operator<<(std::ostream& ostr, const Parameter& param)
    {
      ostr << param.toString();
      return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Parameter* param)
    {
      ostr << param->toString();
      return ostr;
    }

    virtual std::string type() const = 0;

    // arithmetic operations.
    virtual Parameter& operator+=(const Parameter&)
    {
      std::stringstream str;
      str << "operator+ not defined for parameter of type '" << type();
      throw Error(str.str());
    }

    virtual bool operator<=(const Parameter&)
    {
      std::stringstream str;
      str << "operator<= not defined for parameter of type '" << type();
      throw Error(str.str());
    }

    /** comparison of the values. **/
    virtual bool operator==(const Parameter& other) const = 0;
    virtual bool operator!=(const Parameter& other) const
    {
      return !(*this == other);
    }

  protected:
    Parameter(const std::string& n) :
        mName(n)
    {
    }
    std::string mName;
};

}
#endif /* PARAMETER_H_ */
