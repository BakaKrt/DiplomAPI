/*
Используется библиотека nanobench для замера производительности
*/
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"


import std;

import test;
import normalsum;
import iter;
import sse;

using std::array;
using std::string;
using std::vector;
using std::unique_ptr, std::make_unique;

constexpr int SIZE = 100;
constexpr int iterations = 1000; // Количество прогонов для измерения



template<typename T, size_t N>
static std::vector<std::array<T, N>> generateTestData() {
	vector<array<T, N>> objects{ SIZE };
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1000.0f, 1000.0f);

	array<T, N> arr{};

	for (auto& vec : objects) {
		for (auto& a : arr) {
			a = (T)dis(gen);
		}
		vec = arr;
	}

	return objects;
}


template<typename T, size_t N>
static void inline runBenchmark(vector<unique_ptr<test>>& tests, vector<array<T, N>>& testData, const int iterations) noexcept;

/// <summary>
/// Проверить, одинаковые ли array<uint8_t, N> поэлементно
/// </summary>
/// <typeparam name="N"></typeparam>
/// <param name="vec"></param>
/// <returns></returns>
template<typename T, size_t N>
static bool inline checkArraysDifference(vector<array<T, N>>& vec) noexcept {
	if (vec.empty()) return true;

	const size_t size = vec.size();
	if (size == 1) return true;

	bool res = true;

	for (size_t idx = 0; idx < N; ++idx) {
		uint8_t reference_value = vec[0][idx];

		for (size_t i = 1; i < size; ++i) {
			if (vec[i][idx] != reference_value) {
				printf("findex diff at index %4u:", (unsigned int)idx);
				for (size_t j = 0; j < size; ++j) {
					printf("[%u] = %3u ", j, vec[j][idx]);
				}
				printf("\n");
				res = false;
				break;
			}
		}
	}

	return res;
}

template<typename T, size_t testSize, size_t resultSize>
static void inline runTest(vector<unique_ptr<test>>& tests, array<T, testSize> testData) {
	vector<array<T, resultSize>> results{}; results.reserve(tests.size());

	for (auto& test : tests) {
		results.push_back(test->run(testData));
	}

	checkArraysDifference(results);
}

int main() {
	//auto  testData3 = generateTestData<float, 3>();
	//auto  testData5 = generateTestData<float, 5>();
	//auto  testData8 = generateTestData<float, 8>();
	//auto testData12 = generateTestData<float, 12>();
	//auto testData15 = generateTestData<float, 15>();
	auto testData24 = generateTestData<float, 24>();

	//auto   uintData8 = generateTestData<uint8_t, 12>();
	auto uintData112 = generateTestData<uint8_t, 112>();
	auto uintData256 = generateTestData<uint8_t, 256>();
	

	constexpr int TEST_ELEM_COUNT = 3;

	vector<unique_ptr<test>> tests{}; tests.reserve(TEST_ELEM_COUNT);


	tests.push_back(make_unique<NormalSum>());
	tests.push_back(make_unique<IterSum>());
	tests.push_back(make_unique<SSEv1Sum>());

	runTest<  float,  24,   9>(tests,  testData24[0]);
	runTest<uint8_t, 112,  70>(tests, uintData112[0]);
	runTest<uint8_t, 256, 126>(tests, uintData256[0]);


	
	//#define TIMEBASED
	#define NANOBENCH



#if defined(TIMEBASED) && defined(NANOBENCH)
#undef TIMEBASED
#endif

#if !defined(TIMEBASED) && !defined(NANOBENCH)
#define TIMEBASED
#endif

	//runBenchmark(tests, testData3, iterations*10);
	//runBenchmark(tests, testData5, iterations*5);
	//runBenchmark(tests, testData8, iterations*2);
	
	//runBenchmark(tests, uintData8, iterations*2);

	runBenchmark(tests, uintData112, iterations);
	runBenchmark(tests, uintData256, iterations);
	


	//runBenchmark(tests, testData15, iterations);
	//runBenchmark(tests, testData24, iterations);

#ifdef NDEBUG
	std::printf("end");
	std::cin.get();
#endif // RELEASE


}


template<typename T, size_t N>
static void inline runBenchmark(vector<unique_ptr<test>>& tests, vector<array<T, N>>& testData, const int iterations) noexcept {
	printf("test for size %d\n", (int)N);
	for (auto& test : tests) {
#ifdef NANOBENCH
		ankerl::nanobench::Bench().minEpochIterations(5).warmup(10).run(test->getName(), [&] {
#endif // NANOBENCH
#ifdef TIMEBASED
			auto start = std::chrono::high_resolution_clock::now();
#endif // TIMEBASED
			for (int iter = 0; iter < iterations; ++iter) {
				for (auto& vec : testData) {
					test->run(vec);
				}
			}
#ifdef NANOBENCH
			});
#endif // NANOBENCH
#ifdef TIMEBASED
		auto end = std::chrono::high_resolution_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		printf("%s took:\t%7lld us  | iter/us %f\n", test->getName().c_str(), time, static_cast<float>((float)iterations / (float)time));
#endif // TIMEBASED

	}
}