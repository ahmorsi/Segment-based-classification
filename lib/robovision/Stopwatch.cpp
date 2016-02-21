// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#include "rv/Stopwatch.h"

#include <cassert>
#include <stdexcept>
#include <sys/time.h>

namespace rv
{

std::vector<struct timeval> Stopwatch::stimes = std::vector<struct timeval>();

void Stopwatch::tic()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  stimes.push_back(tv);
}

/** \brief stops the last timer started and outputs \a msg, if given.
 *
 *  \return elapsed time in seconds.
 **/
double Stopwatch::toc()
{
  assert(stimes.begin() != stimes.end());
  if (stimes.begin() == stimes.end()) throw std::logic_error("No Stopwatch to stop.");

  struct timeval end;
  gettimeofday(&end, 0);
  struct timeval start = stimes.back();
  stimes.pop_back();

  return (end.tv_sec - start.tv_sec + 1e-6 * (end.tv_usec - start.tv_usec));
}

uint32_t Stopwatch::numRunning()
{
  return stimes.size();
}

}
