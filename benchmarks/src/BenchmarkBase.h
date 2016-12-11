
//          Copyright Sebastian Jeckel 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>
#include <cfloat>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>

#include "react/common/Util.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Get unique random numbers from range.
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, typename TGen>
const std::vector<T> GetUniqueRandomNumbers(TGen gen, T from, T to, T count)
{
    std::vector<T>    data(1 + to - from);
    int c = from;
    for (auto& p : data)
        p = c++;

    for (T i=0; i<count; i++)
    {
        std::uniform_int_distribution<T> dist(i,(to - from));
        auto r = dist(gen);
        
        if (r != i)
            std::swap(data[i], data[r]);
    }
    data.resize(count);
    std::sort(data.begin(), data.end());

    return std::move(data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Get current date/time
///////////////////////////////////////////////////////////////////////////////////////////////////
inline const std::string CurrentDateTime()
{
    char       buf[80];
    time_t     now = time(0);
    struct tm  tstruct = *localtime(&now);
 
    strftime(buf, sizeof(buf), "%Y-%m-%d___%H.%M.%S", &tstruct);
 
    return buf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// BenchmarkBase
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename D>
class BenchmarkBase
{
public:
    typedef D Domain;

    double        Run();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// RunBenchmark
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
    int RUN_COUNT,
    typename TBenchmark,
    typename TParams
>
void RunBenchmark(std::ostream& logfile, TBenchmark b, const TParams& params)
{
    std::cout   << "Engine: " << typeid(typename TBenchmark::Domain::Policy::Engine).name() << std::endl << std::endl;
    logfile     << "Engine: " << typeid(typename TBenchmark::Domain::Policy::Engine).name() << std::endl << std::endl;

    double sum = 0;
    double min = DBL_MAX;
    double max = DBL_MIN;

    for (int i=1; i<=RUN_COUNT; i++)
    {
        double r = b.Run(params);
        std::cout    << "\tRun " << i << ": " <<  r << std::endl;
        logfile        << "\tRun " << i << ": " <<  r << std::endl;
        
        sum += r;

        if (r > max)
            max = r;
        if (r < min)
            min = r;
    }
    double avg = sum / RUN_COUNT;

    std::cout << std::endl;
    std::cout << "\tAverage: " << avg << std::endl;
    std::cout << "\tMin: " << min << std::endl;
    std::cout << "\tMax: " << max << std::endl << std::endl;

    logfile    << std::endl;
    logfile    << "\tAverage: " << avg << std::endl;
    logfile << "\tMin: " << min << std::endl;
    logfile << "\tMax: " << max << std::endl << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// RunBenchmarkClass
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
    int RUN_COUNT,
    template <typename> class TBenchmark,
    typename TParams,
    typename ... Ds
>
void RunBenchmarkClass(const char* name, std::ostream& out, const TParams& params)
{
    std::cout    << "===== " << name << " (";
    params.Print(std::cout);
    std::cout    << ") =====" << std::endl << std::endl;

    out    << "===== " << name << " (";
    params.Print(out);
    out    << ") =====" << std::endl << std::endl;

    REACT_EXPAND_PACK(RunBenchmark<RUN_COUNT>(out, TBenchmark<Ds>(), params));
}

#define RUN_BENCHMARK(out, runCount, benchmarkClass, params, ...) \
    RunBenchmarkClass<runCount, benchmarkClass, decltype(params), __VA_ARGS__>(#benchmarkClass, out, params)
