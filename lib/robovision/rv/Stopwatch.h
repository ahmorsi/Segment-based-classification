// This file is part of the robovision library, (c) Jens Behley, 2015.
//
// The code is provided for educational purposes in the lecture
//        "Knowledge-based Image Understanding", summer term 2015,
// and shall not be redistributed or used in commercial products.

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <vector>
#include <stdint.h>
#include <time.h>

namespace rv
{

/**
 * \headerfile Stopwatch.h
 *
 * \brief simple timing of function calls.
 *
 * Mimics the behavior of tic/toc of Matlab and throws an error if toc() is called without starting a timer before.
 *
 * Example:
 *   Stopwatch::tic(); starts timer A
 *
 *   Stopwatch::tic(); starts timer B
 *   method1();
 *   double time1 = Stopwatch::toc(); stops timer B and returns time elapsed since timer B started
 *
 *   Stopwatch::tic(); starts timer C
 *   method2();
 *   double time2 = Stopwatch::toc(); stops  timer C
 *
 *   Stopwatch::toc("finished"); stops timer A, thus this is approx. time1 + time2. and outputs a message.
 *
 * \author: behley
 */
class Stopwatch
{
  public:
    /** \brief starts a timer. **/
    static void tic();
    /** \brief stops the last timer started and outputs \a msg **/
    static double toc();

    /** \brief number of running stopwatches. **/
    static uint32_t numRunning();
  protected:
    static std::vector<struct timeval> stimes;
};

}

#endif /* STOPWATCH_H_ */
