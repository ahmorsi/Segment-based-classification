// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef STRINGTOKENIZER_H_
#define STRINGTOKENIZER_H_

#include <boost/tokenizer.hpp>

namespace rv
{

/** \brief wrapper for boost::tokenizer offering some convenience methods.
 *
 *  \author behley
 */
class StringTokenizer
{
  public:
    StringTokenizer(const std::string& line, const std::string& delim, bool skipEmpty = false);

    bool hasMoreTokens() const;
    std::string getNextToken();

  protected:
    typedef boost::tokenizer<boost::char_separator<char> > CharTokenizer;
    CharTokenizer tokenizer_;
    CharTokenizer::iterator iterator_;
};

} /* namespace rv */
#endif /* STRINGTOKENIZER_H_ */
