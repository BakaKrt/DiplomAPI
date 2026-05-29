export module benchmarkTotalRealizations;

import std;

import TotalNaiveRealization;
import OnlyAvxRealization;
import TotalOnlyScalarRealization;
import AvxBufferedRealization;

import benchmark;

import random;

export using TotalRealizationsVariant = std::variant<
	TotalOnlyAvxRealization, OnlyScalarRealization, AvxBufferedRealization, NaiveRealization
>;

export struct TotalRealizationTestStruct {
	TotalRealizationsVariant variant;

	// Явный конструктор для типов, отличных от AnyTest
	template<typename T>
	TotalRealizationTestStruct(T&& obj) requires (!std::same_as<std::decay_t<T>, TotalRealizationTestStruct>)
		: variant(std::forward<T>(obj)) {}

	// Конструктор копирования
	TotalRealizationTestStruct(const TotalRealizationTestStruct&) = default;

	// Конструктор перемещения
	TotalRealizationTestStruct(TotalRealizationTestStruct&&) = default;

	// Операторы присваивания
	TotalRealizationTestStruct& operator=(const TotalRealizationTestStruct&) = default;
	TotalRealizationTestStruct& operator=(TotalRealizationTestStruct&&) = default;

	auto getName() const -> decltype(auto) {
		return std::visit([] (const auto& obj) -> decltype(auto) { return obj.getNameImpl(); }, variant);
	}

	auto run(Flat2DArray<uint8_t>& from, Flat2DArray<uint8_t>& to) -> decltype(auto) {
		return std::visit([&from, &to] (auto& obj) -> decltype(auto) { return obj.runImpl(from, to); }, variant);
	}
};

export
void runBenchmarkForTotalRealizations(
	MyBenchmarkNS::BenchmarkParametr param,
	std::vector<TotalRealizationTestStruct>& tests,
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
					test_variant.run(originalArray[i], neighbours_copy[i]);
				}
		});
		BenchmarkResult normalizedResult = rawTimings / static_cast<double>(arraysCount);

		results.push_back(normalizedResult);
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	MyBenchmarkNS::printBenchmarkResults(results, MyBenchmarkNS::SaveTime::microseconds);
}