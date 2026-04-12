#pragma once


#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"


#include "CG_base.hpp"
#include "CG_std_vector.hpp"
#include "CG_std_vector_for.hpp"
#include "CG_std_vector_sse.hpp"

#include "Flat/CG_flat_sse.hpp"
#include "Flat/CG_flat_sum.hpp"


using namespace Benchmark;

using std::vector;
using std::unique_ptr;
using std::make_unique;

struct benchmarkSettings {
    size_t sizeX;
    size_t sizeY;
    int threadsCount = 2;
    int count = 1000;
    bool multiThread = true;
    bool randInit = true;
};

static void test(benchmarkSettings settings) {
    vector<unique_ptr<CaveGeneratorBench>> caves;

    constexpr int CAVES_COUNT = 2;

    caves.reserve(CAVES_COUNT);

    caves.push_back(make_unique<CaveGenerator_vector>(settings.sizeX, settings.sizeY, settings.threadsCount, settings.randInit));
    caves.push_back(make_unique<CaveGenerator_vector_for>(settings.sizeX, settings.sizeY, settings.threadsCount, settings.randInit));
    caves.push_back(make_unique<CaveGenerator_vector_sse>(settings.sizeX, settings.sizeY, settings.threadsCount, settings.randInit));

    caves.push_back(make_unique<CaveGenerator_flat_sum>(settings.sizeX, settings.sizeY, settings.threadsCount, settings.randInit));
    //caves.push_back(make_unique<CaveGenerator_flat_sse>(settings.sizeX, settings.sizeY, settings.threadsCount, settings.randInit));


    for (auto& cave : caves) {
        cave->SetB(2, 3);
        cave->SetS(3, 3);

        ankerl::nanobench::Bench()
            .minEpochIterations(2)
            .run(cave->getName(), [&]
            {
                cave->TickMT(20);
            }
        );
    }
}