import normalsum;
import iter;
import sse_vertical;


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

constexpr int SIZE = 100;

// Количество прогонов для измерения
constexpr int iterations = 1000;


int main() {
	printf("%s\n", InstructionSet::Brand().c_str());
	
	printf("support SSE4.1? = %s\n", InstructionSet::SSE41() ? "true" : "false");


#ifdef _DEBUG
	playgroundTest3();
#endif // _DEBUG
	
	//return 0;
#if defined(NDEBUG)
	{
		constexpr size_t width = 300, height = 300;

		vector<FilterRealizationTestStruct> tests {}; tests.reserve(2);
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
		auto testMemUint8 = generateVectorOfTestMemory<uint8_t>(10, 512, 512);
		//auto testMemFloat = generateVectorOfTestMemory< float >(4, 16 * 2, 3);

		constexpr int TEST_ELEM_COUNT = 3;
		vector<AnyTest> tests {}; tests.reserve(TEST_ELEM_COUNT);

		tests.emplace_back(NormalSum {});
		tests.emplace_back(IterSum {});
		tests.emplace_back(SSEv1Sum {});

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