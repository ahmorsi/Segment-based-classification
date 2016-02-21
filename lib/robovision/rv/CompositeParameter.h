// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef COMPOSITEPARAMETER_H_
#define COMPOSITEPARAMETER_H_

#include "Parameter.h"
#include "ParameterList.h"

namespace rv
{

/**\brief Parameter as a composition of other Parameters.
 *
 *
 * \author behley
 **/
class CompositeParameter: public Parameter
{
	public:
		CompositeParameter(const std::string& name);
		CompositeParameter(const std::string& name, const tinyxml2::XMLElement& value);
		CompositeParameter(const std::string& name, const ParameterList& values);

		CompositeParameter(const CompositeParameter& other);
		virtual CompositeParameter& operator=(const CompositeParameter& other);

		virtual ~CompositeParameter();

		virtual CompositeParameter* clone() const;

		virtual operator ParameterList() const;

		const ParameterList& getParams() const
		{
			return param_list;
		}

		ParameterList& getParams()
		{
		  return param_list;
		}

		virtual std::string toString() const;

		std::string type() const
		{
			return "composite";
		}

		virtual bool operator==(const Parameter& other) const
		{
			const CompositeParameter* param = dynamic_cast<const CompositeParameter*>(&other);
			if(param == 0) return false;

			return (other.name() == mName && param_list == param->param_list);
		}

	protected:
		ParameterList param_list;
};

CompositeParameter* parseCompositeParameter(const tinyxml2::XMLElement& node);

}
#endif /* COMPOSITEPARAMETER_H_ */
