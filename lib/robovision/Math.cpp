// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Math.h"
#include <list>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "rv/IOError.h"
#include "rv/Random.h"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include "rv/string_utils.h"

namespace rv
{

namespace Math
{

struct SortElement
{
    SortElement(unsigned int ind, double val) :
        index(ind), value(val)
    {
    }

    bool operator<(const SortElement& other)
    {
      return (value < other.value);
    }

    unsigned int index;
    double value;
};

void save(const std::string& filename, const std::string& varname, const Eigen::MatrixXf& mat)
{
  std::ofstream out(filename.c_str());
  assert(out.is_open());

  /** outputting the header information. **/
  out << "# name: " << varname << std::endl;
  out << "# type: matrix" << std::endl;
  out << "# rows: " << mat.rows() << std::endl;
  out << "# columns: " << mat.cols() << std::endl;

  for (uint32_t r = 0; r < mat.rows(); ++r)
  {
    for (uint32_t c = 0; c < mat.cols(); ++c)
    {
      out << " " << mat(r, c);
    }
    out << std::endl;
  }

  out.close();
}

bool load(const std::string& filename, Eigen::MatrixXf& M)
{
  std::ifstream in(filename.c_str());
  if (!in.is_open())
  {
    std::cerr << "unable to open '" << filename << "'" << std::endl;
    return false;
  }

  std::string line;
  std::vector<std::string> tokens;

  bool rows_read = false, cols_read = false;
  uint32_t rows = 0, cols = 0;

  in.peek();
  while ((!rows_read || !cols_read) && !in.eof())
  {
    std::getline(in, line);
    if (line[0] != '#') break;
    tokens = split(line, ":");
    // trim = remove "# " in the begining.
    std::string name = trim(tokens[0], " #");
    std::string value = trim(tokens[1]);

    if (name == "type")
    {
      if (value != "matrix")
      {
        std::cerr << "unknown type '" << tokens[1] << "'!" << std::endl;
        return false;
      }
    }

    if (name == "rows")
    {
      rows_read = true;
      // trim = remove leading spaces
      rows = boost::lexical_cast<uint32_t>(value);
    }

    if (name == "columns")
    {
      cols_read = true;
      // trim = remove leading spaces
      cols = boost::lexical_cast<uint32_t>(value);
    }
    in.peek();
  }

  if (!rows_read)
  {
    std::cerr << "missing 'rows' entry!" << std::endl;
    return false;
  }
  if (!cols_read)
  {
    std::cerr << "missing 'cols' entry!" << std::endl;
    return false;
  }

  M = Eigen::MatrixXf(rows, cols);

  in.peek();
  for (uint32_t r = 0; r < rows; ++r)
  {
    if (in.eof()) return false;

    std::getline(in, line);
    // trim = remove leading space.
    tokens = split(trim(line));
    if (tokens.size() < cols) return false;

    for (uint32_t c = 0; c < cols; ++c)
      M(r, c) = boost::lexical_cast<float>(tokens[c]);

    in.peek();
  }

  in.close();

  return true;
}

void save(const std::string& filename, const std::string& varname, const Eigen::MatrixXd& mat)
{
  std::ofstream out(filename.c_str());
  assert(out.is_open());

  /** outputting the header information. **/
  out << "# name: " << varname << std::endl;
  out << "# type: matrix" << std::endl;
  out << "# rows: " << mat.rows() << std::endl;
  out << "# columns: " << mat.cols() << std::endl;

  for (uint32_t r = 0; r < mat.rows(); ++r)
  {
    for (uint32_t c = 0; c < mat.cols(); ++c)
    {
      out << " " << mat(r, c);
    }
    out << std::endl;
  }

  out.close();
}

bool load(const std::string& filename, Eigen::MatrixXd& M)
{
  std::ifstream in(filename.c_str());
  if (!in.is_open())
  {
    std::cerr << "unable to open '" << filename << "'" << std::endl;
    return false;
  }

  std::string line;
  std::vector<std::string> tokens;

  bool rows_read = false, cols_read = false;
  uint32_t rows = 0, cols = 0;

  in.peek();
  while ((!rows_read || !cols_read) && !in.eof())
  {
    std::getline(in, line);
    if (line[0] != '#') break;
    tokens = split(line, ":");
    // trim = remove "# " in the begining.
    std::string name = trim(tokens[0], " #");
    std::string value = trim(tokens[1]);

    if (name == "type")
    {
      if (value != "matrix")
      {
        std::cerr << "unknown type '" << tokens[1] << "'!" << std::endl;
        return false;
      }
    }

    if (name == "rows")
    {
      rows_read = true;
      // trim = remove leading spaces
      rows = boost::lexical_cast<uint32_t>(value);
    }

    if (name == "columns")
    {
      cols_read = true;
      // trim = remove leading spaces
      cols = boost::lexical_cast<uint32_t>(value);
    }
    in.peek();
  }

  if (!rows_read)
  {
    std::cerr << "missing 'rows' entry!" << std::endl;
    return false;
  }
  if (!cols_read)
  {
    std::cerr << "missing 'cols' entry!" << std::endl;
    return false;
  }

  M = Eigen::MatrixXd(rows, cols);

  in.peek();
  for (uint32_t r = 0; r < rows; ++r)
  {
    if (in.eof()) return false;

    std::getline(in, line);
    // trim = remove leading space.
    tokens = split(trim(line));
    if (tokens.size() < cols) return false;

    for (uint32_t c = 0; c < cols; ++c)
      M(r, c) = boost::lexical_cast<double>(tokens[c]);

    in.peek();
  }

  in.close();

  return true;
}

double normal_pdf(double x, double mu, double sigma)
{
  static const double sqrt2pi = std::sqrt(2.0 * PI);

  double value = 1. / (sigma * sqrt2pi);
  value *= std::exp(-0.5 * (x - mu) * (x - mu) / (sigma * sigma));

  return value;
}

double exponential_pdf(double x, double lambda)
{
  return lambda * exp(-lambda * x);
}

double sigmoid2(double x, double loc, double scale)
{
  return 1.0 / (1.0 + std::exp(-scale * x + scale * loc));
}

/** \brief get range of numbers
 *  \return vector containing [0,...,end-1] **/
std::vector<uint32_t> range(uint32_t end)
{
  std::vector<uint32_t> vec(end);
  for (uint32_t i = 0; i < end; ++i)
    vec[i] = i;

  return vec;
}

/** \brief get range of numbers
 *  \return vector containing [start,...,end] **/
std::vector<uint32_t> range(uint32_t start, uint32_t end)
{
  const uint32_t size = end - start;

  std::vector<uint32_t> vec(end - start);
  for (uint32_t i = 0; i < size; ++i)
    vec[i] = i + start;

  return vec;
}

}

}
