#define TEST
#ifdef TEST
#include "Tests/Tests.hpp"
#endif // TEST

//#define BENCHMARK
#ifdef BENCHMARK
#include "ForBenchmarks/Benchmark.hpp"
#endif // BENCHMARK


import std;




int main()
{
    setlocale(0, "");
#ifdef BENCHMARK
    benchmarkSettings settings{
        .sizeX = 128,
        .sizeY = 128,
        .threadsCount = 2,
    };
    test(settings);
#endif // BENCHMARK

    auto cave = CaveGenerator_flat_sse(15, 5, 1, true);
    std::cout << cave;
    cave.TickMT();


#ifdef TEST
    Test::Run();
#endif // TEST



}