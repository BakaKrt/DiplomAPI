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
	playgroundTest0();
#endif // _DEBUG
	
	return 0;
//#if defined(NDEBUG)
	{
		constexpr size_t width = 100, height = 100;

		vector<FilterRealizationTestStruct> tests {}; tests.reserve(2);
		tests.emplace_back(SseRule {});
		tests.emplace_back(NormalRule {});
		tests.emplace_back(AvxRule {});
		
		
		

		printf("support AVX2? = %s\n", InstructionSet::AVX2() ? "true" : "false");
		runBenchmarkForFilters(width, height, tests, 10, "test");
	}
//#endif // RELEASEs 



#define RUN_SUM_TESTS 0
#if defined(NDEBUG) && defined(RUN_SUM_TESTS) && RUN_SUM_TESTS == 1
	{
		auto testMemUint8 = generateVectorOfTestMemory<uint8_t>(4, 16 * 20 + 8, 16 * 5);
		auto testMemFloat = generateVectorOfTestMemory< float >(4, 16 * 2, 3);

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

		//   runBenchmark(tests, testMemUint8, iterations,
		   //	&runHorizontalBenchmark<uint8_t>,
		   //	"horizontal"
		   //);

		//   runBenchmark(tests, testMemUint8, iterations,
		   //	&runHorizontalNextLineSum<uint8_t>,
		   //	"horizontalNextLine"
		   //);

		//   runBenchmark(tests, testMemUint8, iterations,
		   //	&runVerticalBenchmark<uint8_t>,
		   //	"vertical"
		   //);

		std::printf("end");
		std::cin.get();
	}
#endif // RELEASE
}