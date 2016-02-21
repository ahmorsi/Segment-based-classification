// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/StringTokenizer.h"

namespace rv
{

StringTokenizer::StringTokenizer(const std::string& line, const std::string& delim, bool skipEmpty) :
    tokenizer_(line,
        boost::char_separator<char>(delim.c_str(), "",
            ((skipEmpty) ? boost::drop_empty_tokens : boost::keep_empty_tokens))), iterator_(tokenizer_.begin())
{

}

bool StringTokenizer::hasMoreTokens() const
{
  return (iterator_ != tokenizer_.end());
}

std::string StringTokenizer::getNextToken()
{
  std::string nextToken = *iterator_;
  ++iterator_;

  return nextToken;
}

} /* namespace rv */
