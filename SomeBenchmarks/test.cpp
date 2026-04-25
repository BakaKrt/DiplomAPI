#include <variant>
#include <chrono>
#include <functional>
#include <type_traits>


#define BENCHMARK_MODE 1  // 1 = TIMEBASED, 2 = NANOBENCH, 0 = NONE

// В коде:
#if BENCHMARK_MODE == 2
	#define NANOBENCH

	#define ANKERL_NANOBENCH_IMPLEMENT
	#include "nanobench.h"
#elif BENCHMARK_MODE == 1
	#define TIMEBASED
#elif BENCHMARK_MODE == 0
	// Без бенчмарка
#else
	#error "Неизвестный BENCHMARK_MODE"
#endif



import test;
import normalsum;
import iter;
import sse;

import Flat2DArray;
import random;

import cpuinfo;

import std;

using std::array;
using std::string;
using std::vector;
using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;

using std::is_same_v;

using std::cout;

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
	inline auto run_horizontal(Flat2DArray<U>& arg) -> decltype(auto) {
		return std::visit([&arg] (auto& obj) -> decltype(auto) { return obj.runHorizontalSum(arg); }, variant);
	}

	template<typename U>
	inline auto run_horizontalNextLine(Flat2DArray<U>& arg) -> decltype(auto) {
		return std::visit([&arg] (auto& obj) -> decltype(auto) { return obj.runHorizontalNextLineSum(arg); }, variant);
	}

	template<typename U>
	inline auto run_vertical(U& arg) -> decltype(auto) {
		return std::visit([&arg] (auto& obj) -> decltype(auto) { return obj.runVerticalSum(arg); }, variant);
	}

	template<typename U>
	inline auto run_sumAll(U& arg) -> decltype(auto) {
		return std::visit([&arg] (auto& obj) -> decltype(auto) { return obj.test_run(arg); }, variant);
	}
};



template <typename T>
static shared_ptr<Flat2DArray<T>> generateTestMemory(size_t width, size_t height) {
	auto obj = make_shared<Flat2DArray<T>>(width, height, false);
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


template<typename Fn, typename T>
void runBenchmark(
	std::vector<AnyTest>& tests,
	const std::vector<std::shared_ptr<Flat2DArray<T>>>& testData,
	const int iterations,
	Fn&& execute_test,
	const char* benchName
) {
	printf("benchmark %s:\n", benchName);

#ifdef TIMEBASED
	std::vector<std::pair<std::string, long long>> pairs;
	pairs.reserve(tests.size());
#endif

	for (auto& test_variant : tests) {
		string name = test_variant.getName();

#ifdef NANOBENCH
		ankerl::nanobench::Bench().minEpochIterations(10).warmup(10).run(name, [&] {
#endif
#ifdef TIMEBASED
			auto start = std::chrono::high_resolution_clock::now();
#endif // TIMEBASED
            // Вызов измерения с помощью переданной функции.
			// Поддерживаем две сигнатуры execute_test:
			//  - Fn(AnyTest&, Flat2DArray<T>&)
			//  - Fn(AnyTest&, Flat2DArray<T>&, int)  <-- получает номер итерации
			for (int iter = 0; iter < iterations; ++iter) {
				for (auto& vec : testData) {
					if constexpr (std::is_invocable_v<Fn, AnyTest&, Flat2DArray<T>&, int>) {
						std::invoke(execute_test, test_variant, *vec, iter);
					}
					else if constexpr (std::is_invocable_v<Fn, AnyTest&, Flat2DArray<T>&>) {
						std::invoke(execute_test, test_variant, *vec);
					}
					else {
						static_assert(std::is_invocable_v<Fn, AnyTest&, Flat2DArray<T>&> ||
									  std::is_invocable_v<Fn, AnyTest&, Flat2DArray<T>&, int>,
									  "execute_test must be callable as (AnyTest&, Flat2DArray<T>&) or (AnyTest&, Flat2DArray<T>&, int)");
					}
				}
			}

#ifdef TIMEBASED
			auto end = std::chrono::high_resolution_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			printf("%s took:\t%7lld us  | iter/us %f\n", name.c_str(), time, static_cast<float>((float) iterations / (float) time));
			pairs.push_back(make_pair(name, time));
#endif // TIMEBASED
#ifdef NANOBENCH
		});
#endif
	}

#ifdef TIMEBASED
	// Анализ и вывод результатов
	auto min_time = std::min_element(pairs.begin(), pairs.end(),
		[] (const auto& a, const auto& b) { return a.second < b.second; })->second;

	for (const auto& [name, time] : pairs) {
		double speedPercent = 0.0;
		double timePercent = 0.0;
		if (time != 0) {
			speedPercent = (double) min_time / (double) time * 100.0;
			timePercent = (double) time / (double) min_time * 100.0;
		}
		printf("%6s - speed: %6.2f%% - time: %5.2f%%\n", name.c_str(), speedPercent, timePercent);
	}
	printf("\n\n");
#endif
}

template<typename T>
void runHorizontalBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_horizontal(testData);
}

template<typename T>
void runHorizontalNextLineSum(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_horizontalNextLine(testData);
}

template<typename T>
void runVerticalBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_vertical(testData);
}

template<typename T>
void runFullSumBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_sumAll(testData);
}


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

int main() {
	printf("%s\n", InstructionSet::Brand().c_str());
	
	printf("support SSE 4.1? = %s\n", InstructionSet::SSE41() ? "true" : "false");


	auto testMemUint8 = generateVectorOfTestMemory<uint8_t>(4, 16 * 20 + 8, 16 * 5);
	auto testMemFloat = generateVectorOfTestMemory< float >(4, 16 * 2, 3);

#ifdef _DEBUG
	#ifdef testVerical
{
	SSEv1Sum sseTest {}; NormalSum normalTest {}; IterSum iterTest {};
	size_t width = 25, height = 4, capacity = width * height;
	auto mem = Flat2DArray<uint8_t>(width, height);

	for (size_t i = 0; i < capacity; i++) {
		mem[i] = (uint8_t)(i) % 10 + randomUint8(0, 40);
	}

	cout << "init mem width: " << width << " height: " << height << "\n" << mem << "\n";
	auto res = sseTest.runVerticalSum(mem);
	auto normalRes = iterTest.runVerticalSum(mem);

	cout << "res:\n" << res << "\n";
	cout << "normalRes:\n" << normalRes << "\n";

	for (size_t x = 0; x < normalRes.capacity(); x++) {
		if (normalRes[x] != res[x]) {
			printf("got diff index[%3u]: n %u s %u\n", (unsigned)x, normalRes[x], res[x]);
		}
	}

 	//__debugbreak();
}
	#endif // testVerical
	//#define testHorizontalNextLineSum
	#ifdef testHorizontalNextLineSum
{
	SSEv1Sum sseTest {}; NormalSum normalTest {}; IterSum iterTest {};
	size_t width = 6, height = 5, capacity = width * height;
	auto mem = Flat2DArray<uint8_t>(width, height);

	for (size_t i = 0; i < capacity; i++) {
		mem[i] = (uint8_t) (i);
	}

	cout << "init mem width: " << width << " height: " << height << "\n" << mem << "\n";
	auto res = sseTest.runHorizontalNextLineSum(mem);
	auto normalRes = iterTest.runHorizontalNextLineSum(mem);

	cout << "res:\n" << res << "\n";
	cout << "normalRes:\n" << normalRes << "\n";

	for (size_t x = 0; x < normalRes.capacity(); x++) {
		if (normalRes[x] != res[x]) {
			printf("got diff index[%3u]: n %u s %u\n", (unsigned) x, normalRes[x], res[x]);
		}
	}

	__debugbreak();
}
	#endif // testHorizontalNextLineSum

	#define run_test
	#ifdef run_test
	{
		IterSum iterTest {}; SSEv1Sum sseTest {};
		size_t width = 35, height = 5, capacity = width * height;
		auto mem = Flat2DArray<uint8_t>(width, height);

		for (size_t i = 0; i < capacity; i++) {
			mem[i] = (uint8_t) (i%9);
		}

		cout << "init mem width: " << width << " height: " << height << "\n" << mem << "\n";
		auto res = sseTest.test_run(mem);
		auto normalRes = iterTest.test_run(mem);

		cout << "res:\n" << res << "\n";
		cout << "normalRes:\n" << normalRes << "\n";

		for (size_t x = 0; x < normalRes.capacity(); x++) {
			if (normalRes[x] != res[x]) {
				printf("got diff index[%3u]: n %u s %u\n", (unsigned) x, normalRes[x], res[x]);
			}
		}
	}

	#endif // run_test


#endif // _DEBUG


	auto testData24 = generateTestData<float, 24>();

	auto uintData112 = generateTestData<uint8_t, 112>();
	auto uintData256 = generateTestData<uint8_t, 256>();
	

	constexpr int TEST_ELEM_COUNT = 3;
	vector<AnyTest> tests {}; tests.reserve(TEST_ELEM_COUNT);

	tests.emplace_back(NormalSum {});
	tests.emplace_back(IterSum {});
	tests.emplace_back(SSEv1Sum {});

	//__debugbreak();

	//runTest<uint8_t, 112,  70>(tests, uintData112[0]);
	//runTest<uint8_t, 256, 126>(tests, uintData256[0]);


#if defined(BENCHMARK_MODE) && BENCHMARK_MODE != 0 && defined(NDEBUG)
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
#endif // RELEASE
}