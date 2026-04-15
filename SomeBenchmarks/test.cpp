/*
Используется библиотека nanobench для замера производительности
*/
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"
#include <variant>



import std;

import test;
import normalsum;
import iter;
import sse;

import cpuinfo;


import Flat2DArray;
import random;


using std::array;
using std::string;
using std::vector;
using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;

using std::is_same_v;

constexpr int SIZE = 100;

// Количество прогонов для измерения
constexpr int iterations = 1000;


// Тут указать все тесты, которые нужно запускать
using TestVariant = std::variant<NormalSum, IterSum, SSEv1Sum>;

struct AnyTest {
	TestVariant variant;

	// Явный конструктор для типов, отличных от AnyTest
	template<typename T>
	AnyTest(T&& obj) requires (!std::same_as<std::decay_t<T>, AnyTest>)
		: variant(std::forward<T>(obj)) {}

	// Конструктор копирования
	AnyTest(const AnyTest&) = default;

	// Конструктор перемещения
	AnyTest(AnyTest&&) = default;

	// Операторы присваивания
	AnyTest& operator=(const AnyTest&) = default;
	AnyTest& operator=(AnyTest&&) = default;

	std::string getName() const {
		return std::visit([] (const auto& obj) { return obj.getName(); }, variant);
	}

	template<typename U>
	inline auto run(U& arg) -> decltype(auto) {
		return std::visit([&arg] (auto& obj) -> decltype(auto) { return obj.run(arg); }, variant);
	}
};



template <typename T>
static shared_ptr<Flat2DArray<T>> generateTestMemory(size_t width, size_t height) {
	auto obj = make_shared<Flat2DArray<T>>(width, height);
	auto ptr = obj->data();

	const size_t size = width * height;

	if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, bool>) {
		fillArrayRandomBool(size, (bool*)ptr, 50);
	}
	else if constexpr (std::is_same_v<T, float>) {
		fillArrayRandomFloat(size, (float*)ptr, 13);
	}
	else {
		fillArrayRandomInt(size, (int*)ptr, 13);
	}
	return obj;
}

template <typename T>
static vector<shared_ptr<Flat2DArray<T>>> generateVectorOfTestMemory(size_t len, size_t width, size_t height) {
	vector<shared_ptr<Flat2DArray<T>>> vec {}; vec.reserve(len);

	for (size_t i = 0; i < len; i++) {
		vec.push_back(generateTestMemory<T>(width, height));
	}

	return vec;
}

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
static void inline runBenchmark(vector<AnyTest>& tests, vector<array<T, N>>& testData, const int iterations) noexcept;

/// <summary>
/// Проверить, одинаковые ли array<T, N> поэлементно
/// </summary>
/// <typeparam name="N"></typeparam>
/// <param name="vec"></param>
/// <returns></returns>
template<typename T, size_t N>
static bool inline isArraysDifferent(vector<array<T, N>>& vec) noexcept {
	if (vec.empty()) return false;

	const size_t size = vec.size();
	if (size == 1) return false;

	bool res = false;

	for (size_t idx = 0; idx < N; ++idx) {
		T reference_value = vec[0][idx];

		for (size_t i = 1; i < size; ++i) {
			if (vec[i][idx] != reference_value) {
				printf("finded diff at index %4u:", (unsigned int)idx);
				for (size_t j = 0; j < size; ++j) {
					printf("[%u] = %3u ", (unsigned int) j, (T) (vec[j][idx]));
				}
				printf("\n");
				res = true;
				break;
			}
		}
	}

	return res;
}

template<typename T, size_t testSize, size_t resultSize>
static void inline runTest(vector<AnyTest>& tests, array<T, testSize> testData) {
	vector<array<T, resultSize>> results{}; results.reserve(tests.size());

	for (auto& test : tests) {
		results.push_back(test.run(testData));
	}

	bool res = isArraysDifferent(results);
	if (res) {
		std::printf("errors for: ");
		for (auto& test : tests) {
			std::printf("%s ", test.getName().c_str());
		}
		std::printf("and with testSize = %u, testResultSize = %u\n", (unsigned)testSize, (unsigned) resultSize);
	}
}

int main() {
	printf("%s\n", InstructionSet::Vendor().c_str());
	printf("%s\n", InstructionSet::Brand().c_str());
	
	printf("support SSE 4.1? = %s\n", InstructionSet::SSE41() ? "true" : "false");


	auto testMemUint8 = generateVectorOfTestMemory<uint8_t>(SIZE, 16 * 4, 256);
	auto testMemFloat = generateVectorOfTestMemory< float >(SIZE, 16 * 4, 256);



	

	auto testData24 = generateTestData<float, 24>();

	auto uintData112 = generateTestData<uint8_t, 112>();
	auto uintData256 = generateTestData<uint8_t, 256>();
	

	constexpr int TEST_ELEM_COUNT = 3;
	vector<AnyTest> tests {}; tests.reserve(TEST_ELEM_COUNT);

	tests.emplace_back(NormalSum {});
	tests.emplace_back(IterSum {});
	tests.emplace_back(SSEv1Sum {});

	//__debugbreak();

	//runTest<  float,  24,   8>(tests,  testData24[0]);
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

	runBenchmark(tests, uintData112, iterations);
	runBenchmark(tests, uintData256, iterations);


#ifdef NDEBUG
	std::printf("end");
	std::cin.get();
#endif // RELEASE


}


template<typename T, size_t N>
static void inline runBenchmark(vector<AnyTest>& tests, vector<array<T, N>>& testData, const int iterations) noexcept {
	printf("benchmark for size %d\n", (int)N);

	for (auto& test_variant : tests) {
		std::string name = test_variant.getName();

#ifdef NANOBENCH
		ankerl::nanobench::Bench().minEpochIterations(5).warmup(10).run(name, [&] {
#endif // NANOBENCH
#ifdef TIMEBASED
			auto start = std::chrono::high_resolution_clock::now();
#endif // TIMEBASED
			for (int iter = 0; iter < iterations; ++iter) {
				for (auto& vec : testData) {
					test_variant.run(vec);
				}
			}
#ifdef NANOBENCH
			});
#endif // NANOBENCH
#ifdef TIMEBASED
		auto end = std::chrono::high_resolution_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		printf("%s took:\t%7lld us  | iter/us %f\n", name.c_str(), time, static_cast<float>((float)iterations / (float)time));
#endif // TIMEBASED

	}
}