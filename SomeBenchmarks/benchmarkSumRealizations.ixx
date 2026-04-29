export module benchmarkSumRealizations;

import std;
import Flat2DArray;

import normalsum;
import iter;
import sse_vertical;

using std::string;
using std::printf;

using std::is_same_v;

#define BENCHMARK_MODE 1  // 1 = TIMEBASED, 2 = NANOBENCH, 0 = NONE

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


// Тут указать все тесты, которые нужно запускать
export using TestVariant = std::variant<NormalSum, IterSum, SSEv1Sum>;

export struct AnyTest {
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



export
template<typename Fn, typename T>
void runBenchmark(
	std::vector<AnyTest>& tests,
	const std::vector<std::shared_ptr<Flat2DArray<T>>>& testData,
	const int iterations,
	Fn&& execute_test,
	const char* benchName
) noexcept {
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
					if constexpr (std::is_invocable_v<Fn, AnyTest&, Flat2DArray<T>&>) {
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

export template<typename T>
void runHorizontalBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_horizontal(testData);
}

export template<typename T>
void runHorizontalNextLineSum(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_horizontalNextLine(testData);
}

export template<typename T>
void runVerticalBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_vertical(testData);
}

export template<typename T>
void runFullSumBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_sumAll(testData);
}