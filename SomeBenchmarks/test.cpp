import normalsum;
import normalsumv2;
import iter;
import sse_vertical;
import sse_horizontal;

import avx_horizontal;


import benchmarkSumRealizations;
import benchmarkFilterRealizations;

import sseRule;
import normalRule;
import avxRule;

import isItWorkingPlayground;
import Flat2DArray;
import random;
import cpuinfo;
import std;

using std::array;
using std::string;
using std::vector;
using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;


using std::cout, std::printf;

// Количество прогонов для измерения
constexpr int iterations = 10;


int main() {
	printf("%s\n", InstructionSet::Brand().c_str());
	
	printf("support SSE4.1? = %s\n", InstructionSet::SSE41() ? "true" : "false");


#ifdef _DEBUG
	playgroundTest4();
#endif // _DEBUG
	
	//return 0;
#if defined(NDEBUG)
	{
		constexpr size_t width = 300, height = 300;

		vector<FilterRealizationTestStruct> tests {}; tests.reserve(3);
		tests.emplace_back(SseRule {});
		tests.emplace_back(NormalRule {});
		tests.emplace_back(AvxRule {});

		printf("support AVX2? = %s\n", InstructionSet::AVX2() ? "true" : "false");
		runBenchmarkForFilters(width, height, tests, 20, "test");
	}
#endif // RELEASE



#define RUN_SUM_TESTS 1
#if defined(NDEBUG) && defined(RUN_SUM_TESTS) && RUN_SUM_TESTS == 1
	{
		auto testMemUint8 = generateVectorOfTestMemory<uint8_t>(10000, 32, 512, 512);
		//auto testMemFloat = generateVectorOfTestMemory< float >(4, 16 * 2, 3);

		constexpr int TEST_ELEM_COUNT = 6;
		vector<AnyTest> tests {}; tests.reserve(TEST_ELEM_COUNT);

		tests.emplace_back(NormalSum {});
		//tests.emplace_back(NormalWOAllocSum {});
		tests.emplace_back(IterSum {});
		tests.emplace_back(SSEv1Sum {});
		tests.emplace_back(SSEv2Sum {});
		tests.emplace_back(AVXv1HorizontalSum {});

		// запуск бенчмарков только в релизе
		runBenchmark(tests, testMemUint8, iterations,
			&runFullSumBenchmark<uint8_t>,
			"all object sum"
		);

		std::printf("end");
		std::cin.get();
	}
#endif // RELEASE
}

/*
iterations count = 10, arrays count = 10000, sizes: 512 * 512
Algorithm       |        Avg |     StdDev |        Min |        Max |    p0.001% |      p0.1% |        p1% |       p50% |       p99% |     p99.9% |   p99.999%
----------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+-----------
normal          |    0.03579 |    0.00739 |    0.03100 |    0.38900 |    0.03100 |    0.03100 |    0.03200 |    0.03300 |    0.05800 |    0.15200 |    0.26900
iter            |    0.03499 |    0.00687 |    0.03000 |    0.31600 |    0.03000 |    0.03000 |    0.03100 |    0.03200 |    0.05800 |    0.10600 |    0.19500
sse v1          |    0.05310 |    0.01366 |    0.03900 |    0.68400 |    0.03900 |    0.04000 |    0.04100 |    0.04900 |    0.09500 |    0.17800 |    0.36700
sse v2          |    0.02766 |    0.00587 |    0.02200 |    0.30800 |    0.02200 |    0.02300 |    0.02300 |    0.02600 |    0.04900 |    0.07300 |    0.17500
avx h           |    0.01937 |    0.00471 |    0.01600 |    0.30500 |    0.01600 |    0.01700 |    0.01700 |    0.01800 |    0.03500 |    0.05700 |    0.30100

-
normal          |    0.03588 |    0.00749 |    0.03100 |    0.32500 |    0.03100 |    0.03100 |    0.03100 |    0.03300 |    0.06000 |    0.15000 |    0.32400
iter            |    0.03493 |    0.00785 |    0.03000 |    0.38500 |    0.03000 |    0.03000 |    0.03000 |    0.03200 |    0.06000 |    0.13400 |    0.32500
sse v1          |    0.05175 |    0.01289 |    0.03900 |    0.40200 |    0.03900 |    0.04000 |    0.04000 |    0.04800 |    0.09200 |    0.17300 |    0.32200
sse v2          |    0.02755 |    0.00563 |    0.02200 |    0.20300 |    0.02300 |    0.02300 |    0.02300 |    0.02600 |    0.04800 |    0.06800 |    0.18800
avx h           |    0.02003 |    0.00470 |    0.01700 |    0.32900 |    0.01700 |    0.01700 |    0.01700 |    0.01900 |    0.03800 |    0.05500 |    0.17900
-
normal          |    0.03554 |    0.00721 |    0.03100 |    0.32900 |    0.03100 |    0.03100 |    0.03100 |    0.03300 |    0.05900 |    0.15200 |    0.32400
iter            |    0.03470 |    0.00698 |    0.03000 |    0.31700 |    0.03000 |    0.03000 |    0.03100 |    0.03200 |    0.05800 |    0.13400 |    0.24400
sse v1          |    0.05169 |    0.01316 |    0.03900 |    0.33900 |    0.03900 |    0.03900 |    0.04000 |    0.04800 |    0.09300 |    0.17600 |    0.30800
sse v2          |    0.02743 |    0.00598 |    0.02300 |    0.32400 |    0.02300 |    0.02300 |    0.02300 |    0.02600 |    0.04900 |    0.08300 |    0.25200
avx h           |    0.01979 |    0.00526 |    0.01700 |    0.30200 |    0.01700 |    0.01700 |    0.01700 |    0.01800 |    0.03800 |    0.07200 |    0.18800
-
normal          |    0.03568 |    0.00708 |    0.03100 |    0.32200 |    0.03100 |    0.03100 |    0.03100 |    0.03300 |    0.05900 |    0.13100 |    0.31900
iter            |    0.03477 |    0.00700 |    0.03000 |    0.21300 |    0.03000 |    0.03000 |    0.03100 |    0.03200 |    0.05800 |    0.14500 |    0.19300
sse v1          |    0.05214 |    0.01329 |    0.03900 |    0.35600 |    0.03900 |    0.04000 |    0.04000 |    0.04900 |    0.09400 |    0.17400 |    0.34400
sse v2          |    0.02735 |    0.00565 |    0.02200 |    0.31500 |    0.02300 |    0.02300 |    0.02300 |    0.02600 |    0.04800 |    0.06700 |    0.18300
avx h           |    0.01967 |    0.00473 |    0.01600 |    0.30900 |    0.01700 |    0.01700 |    0.01700 |    0.01800 |    0.03700 |    0.05500 |    0.18100
-
normal          |    0.03623 |    0.00655 |    0.03100 |    0.19300 |    0.03100 |    0.03100 |    0.03200 |    0.03300 |    0.06000 |    0.08300 |    0.18100
iter            |    0.03533 |    0.00673 |    0.03000 |    0.31400 |    0.03000 |    0.03000 |    0.03100 |    0.03200 |    0.05900 |    0.07900 |    0.31000
sse v1          |    0.05131 |    0.01230 |    0.03900 |    0.33600 |    0.03900 |    0.04000 |    0.04000 |    0.04800 |    0.09200 |    0.14000 |    0.23400
sse v2          |    0.02855 |    0.00587 |    0.02300 |    0.19000 |    0.02300 |    0.02300 |    0.02400 |    0.02600 |    0.04900 |    0.06800 |    0.18200
avx h           |    0.02031 |    0.00482 |    0.01700 |    0.30300 |    0.01700 |    0.01700 |    0.01700 |    0.01900 |    0.03800 |    0.05700 |    0.17000
*/