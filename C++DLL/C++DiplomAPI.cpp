#define TEST 1
#define BENCH 1


import std;


#if defined(TEST) && TEST == 1
#include "Tests/Tests.hpp"
#endif // TEST


#if !defined(_DEBUG) && defined(BENCH) && BENCH == 1
#include "ForBenchmarks/Benchmark.hpp" // <-- Здесь должно быть включение
#endif // BENCHMARK




int main()
{
    setlocale(0, "");
#if !defined(_DEBUG) && defined(BENCH) && BENCH == 1
    benchmarkSettings settings{
        .sizeX = 128,
        .sizeY = 128,
        .threadsCount = 2,
    };
    test(settings);
#endif // BENCHMARK


#if defined(TEST) && TEST == 1
    Test::Run();
#endif // TEST
}