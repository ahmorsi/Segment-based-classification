// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/string_utils.h"

#include <stdint.h>
#include <iostream>
#include <sstream>

#include "rv/StringTokenizer.h"

namespace rv
{

std::string trim(const std::string& str, const std::string& whitespaces)
{
  int32_t beg = 0;
  int32_t end = 0;

  /** find the beginning **/
  for (beg = 0; beg < (int32_t) str.size(); ++beg)
  {
    bool found = false;
    for (uint32_t i = 0; i < whitespaces.size(); ++i)
    {
      if (str[beg] == whitespaces[i])
      {
        found = true;
        break;
      }
    }
    if (!found) break;
  }

  /** find the end **/
  for (end = int32_t(str.size()) - 1; end > beg; --end)
  {
    bool found = false;
    for (uint32_t i = 0; i < whitespaces.size(); ++i)
    {
      if (str[end] == whitespaces[i])
      {
        found = true;
        break;
      }
    }
    if (!found) break;
  }

  return str.substr(beg, end - beg + 1);
}

std::vector<std::string> split(const std::string& line,
    const std::string& delim, bool skipEmpty)
{
  StringTokenizer tokenizer(line, delim, skipEmpty);
  std::vector<std::string> tokens;

  while (tokenizer.hasMoreTokens())
  {
    tokens.push_back(tokenizer.getNextToken());
  }
  return tokens;
}

}

