export module benchmarkFilterRealizations;

import std;
import Flat2DArray;

import normalRule;
import sseRule;
import avxRule;

import random;




export using FilterRealizationsVariant = std::variant<NormalRule, SseRule, AvxRule>;

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

	std::string getName() const {
		return std::visit([] (const auto& obj) { return obj.getName_impl(); }, variant);
	}

	template<typename U>
	auto applyRule(Flat2DArray<U>& arg, Flat2DArray<U>& neighbours) -> decltype(auto) {
		return std::visit([&arg, &neighbours] (auto& obj) -> decltype(auto) { return obj.applyRule_impl(arg, neighbours); }, variant);
	}
};


export
void runBenchmarkForFilters(
	const size_t width, const size_t height,
	std::vector<FilterRealizationTestStruct>& tests,
	const int iterations,
	const char* benchName
) noexcept {

	using std::printf, std::string;

	printf("benchmark %s:\n", benchName);


	std::vector<std::pair<std::string, long long>> pairs;
	pairs.reserve(tests.size());


	auto originalArray = generateAlignedMemoryForGameOfLife(width, height, iterations, 32, false);
	auto neigboursArray = generateAlignedMemoryForGameOfLife(width, height, iterations, 32, true);

	for (auto& test_variant : tests) {
		string name = test_variant.getName();

		//auto neighbours_copy = generateAlignedMemoryForGameOfLife(width, height, iterations, 32, true);
		auto neighbours_copy(neigboursArray);

		auto start = std::chrono::high_resolution_clock::now();

		for (int iter = 0; iter < iterations; ++iter) {
			test_variant.applyRule(originalArray[iter], neighbours_copy[iter]);
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		printf("%s took:\t%7lld us  | iter/us %f\n", name.c_str(), time, static_cast<float>((float) iterations / (float) time));
		pairs.push_back(make_pair(name, time));

	}

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
}