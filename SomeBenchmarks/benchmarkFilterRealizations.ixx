export module benchmarkFilterRealizations;

import std;
import Flat2DArray;

import normalRule;
import normalOptimizedRule;
import bufferedRule;
import bitsetRule;
import bitsetBufferedRule;

import sseRule;
import avxRule;

import random;

import benchmark;


export using FilterRealizationsVariant = std::variant<
	NormalRule, NormalRuleIfOpt,
	BufferedRule,
	BitsetRule, BitsetBufferedRule,
	SseRule, AvxRule
>;

export struct FilterRealizationTestStruct {
	FilterRealizationsVariant variant;

	// Явный конструктор для типов, отличных от AnyTest
	template<typename T>
	FilterRealizationTestStruct(T&& obj) requires (!std::same_as<std::decay_t<T>, FilterRealizationTestStruct>)
		: variant(std::forward<T>(obj)) {}

	// Конструктор копирования
	FilterRealizationTestStruct(const FilterRealizationTestStruct&) = default;

	// Конструктор перемещения
	FilterRealizationTestStruct(FilterRealizationTestStruct&&) = default;

	// Операторы присваивания
	FilterRealizationTestStruct& operator=(const FilterRealizationTestStruct&) = default;
	FilterRealizationTestStruct& operator=(FilterRealizationTestStruct&&) = default;

	auto getName() const -> decltype(auto) {
		return std::visit([] (const auto& obj) -> decltype(auto) { return obj.getName_impl(); }, variant);
	}

	template<typename U>
	auto applyRule(Flat2DArray<U>& arg, Flat2DArray<U>& neighbours) -> decltype(auto) {
		return std::visit([&arg, &neighbours] (auto& obj) -> decltype(auto) { return obj.applyRule_impl(arg, neighbours); }, variant);
	}
};


export
void runBenchmarkForFilters(
	const size_t width, const size_t height, const size_t warmupCounts,
	std::vector<FilterRealizationTestStruct>& tests,
	const int iterations,
	const char* benchName
) noexcept {
	using namespace MyBenchmarkNS;
	using std::printf, std::string;
	using std::vector;

	printf("benchmark %s:\n", benchName);

	
	vector<BenchmarkResult> results; results.reserve(iterations);

	auto originalArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, iterations, 32, false);
	auto neigboursArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, iterations, 32, true);

	for (auto& test_variant : tests) {
		string name = test_variant.getName();

		auto neighbours_copy(neigboursArray);

		auto timings = MyBenchmarkNS::run(warmupCounts, iterations, name, SaveTime::nanoseconds, [&] () {
			for (int iter = 0; iter < iterations; ++iter) {
				test_variant.applyRule(originalArray[iter], neighbours_copy[iter]);
			}
		});
		results.push_back(timings);
	}
	MyBenchmarkNS::printBenchmarkResults(results, MyBenchmarkNS::SaveTime::microseconds);
}

export
void runBenchmarkForFilters(
	MyBenchmarkNS::BenchmarkParametr param,
	std::vector<FilterRealizationTestStruct>& tests,
	const char* benchName,
	const size_t arraysCount = 10
) noexcept {
	using namespace MyBenchmarkNS;
	using std::printf, std::string;
	using std::vector;

	printf("===== BENCHMARK: %s =====\n", benchName);
	const size_t _width = param.width;
	const size_t _height = param.height;
	printf("iterations count = %zu, arrays count = %zu, sizes: %zu * %zu, ", param.iterations, arraysCount, _width, _height);

	printf("warmup count: %zu\n", param.warmups);

	vector<BenchmarkResult> results; results.reserve(param.iterations);

	auto originalArray = generateVectorOfAlignedMemoryForGameOfLife(param.width, param.height, arraysCount, 32, false);
	auto neigboursArray = generateVectorOfAlignedMemoryForGameOfLife(param.width, param.height, arraysCount, 32, true);

	for (auto& test_variant : tests) {
		string name = test_variant.getName();
		printf("current test name: %s\n", name.c_str());

		std::this_thread::sleep_for(std::chrono::milliseconds(250));

		auto neighbours_copy(neigboursArray);

		auto rawTimings = MyBenchmarkNS::run(
			param.warmups,
			param.iterations,
			name,
			SaveTime::nanoseconds, [&] () {
			for (int i = 0; i < arraysCount; ++i) {
				test_variant.applyRule(originalArray[i], neighbours_copy[i]);
			}
		});
		BenchmarkResult normalizedResult = rawTimings / static_cast<double>(arraysCount);

		results.push_back(normalizedResult);
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	MyBenchmarkNS::printBenchmarkResults(results, MyBenchmarkNS::SaveTime::nanoseconds);
}