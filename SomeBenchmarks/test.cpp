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
import avx;

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
static void inline runTest(vector<unique_ptr<test>>& tests, vector<array<T, N>>& testData, const int iterations) noexcept;

int main() {
	auto  testData3 = generateTestData<float, 3>();
	auto  testData5 = generateTestData<float, 5>();
	auto  testData8 = generateTestData<float, 8>();
	//auto testData12 = generateTestData<float, 12>();
	//auto testData15 = generateTestData<float, 15>();
	//auto testData24 = generateTestData<float, 24>();

	auto   uintData8 = generateTestData<uint8_t, 12>();
	auto uintData256 = generateTestData<uint8_t, 256>();

	constexpr int TEST_ELEM_COUNT = 5;

	vector<unique_ptr<test>> tests{}; tests.reserve(TEST_ELEM_COUNT);


	tests.push_back(make_unique<NormalSum>());
	tests.push_back(make_unique<IterSum>());
	tests.push_back(make_unique<SSEv1Sum>());
	//tests.push_back(make_unique<AVXSum>()); 

#ifdef _DEBUG 

#pragma region Check256
	{
		array<uint8_t, 256> arr{};

		short c = 0;
		for (auto& a : arr) {
			a = (uint8_t)(c++) % 16;
		}

		auto res = tests.back()->run(arr);
		auto resNormal = tests[1]->run(arr);

		for (size_t x = 0; x < res.size(); x++) {
			if (res[x] != resNormal[x]) {
				printf("difference at index %u! got %u and %u\n", (unsigned int)x, res[x], resNormal[x]);


				(void)res;
			}
		}
	}
#pragma endregion

#pragma region Check112
	{
		array<uint8_t, 112> arr{};
		short c = 0;
		for (auto& a : arr) {
			a = (uint8_t)(c++) % 16;
		}

		auto res = tests.back()->run(arr);
		auto resNormal = tests[1]->run(arr);

		for (size_t x = 0; x < res.size(); x++) {
			if (res[x] != resNormal[x]) {
				printf("difference at index %u! got %u and %u\n", (unsigned int)x, res[x], resNormal[x]);


				(void)res;
			}
		}
	}
#pragma endregion



#endif // DEBUG
	
	//#define TIMEBASED
	#define NANOBENCH



#if defined(TIMEBASED) && defined(NANOBENCH)
#undef TIMEBASED
#endif

#if !defined(TIMEBASED) && !defined(NANOBENCH)
#define TIMEBASED
#endif

	runTest(tests, testData3, iterations*10);
	runTest(tests, testData5, iterations*5);
	runTest(tests, testData8, iterations*2);
	//runTest(tests, testData12, iterations*2);
	
	runTest(tests, uintData8, iterations*2);
	runTest(tests, uintData256, iterations);


	//runTest(tests, testData15, iterations);
	//runTest(tests, testData24, iterations);

}


template<typename T, size_t N>
static void inline runTest(vector<unique_ptr<test>>& tests, vector<array<T, N>>& testData, const int iterations) noexcept {
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