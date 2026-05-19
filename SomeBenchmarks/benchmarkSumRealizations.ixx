module;

#include <chrono>

export module benchmarkSumRealizations;

import std;

import random;

import Flat2DArray;

import normalsum;
import normalsumv2;

import iter;

import sse_vertical;
import sse_horizontal;
import avx_horizontal;

import benchmark;

using std::string;
using std::vector;
using std::printf;

using std::is_same_v;


// Тут указать все тесты, которые нужно запускать
export using TestVariant = std::variant<NormalSum, NormalWOAllocSum, IterSum, SSEv1Sum, SSEv2Sum, AVXv1HorizontalSum>;

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
	inline auto run_sumAll(U& arg) -> decltype(auto) {
		return std::visit([&arg] (auto& obj) -> decltype(auto) { return obj.test_run(arg); }, variant);
	}

	template<typename U>
	inline auto run_sumAll(U& arg, U& second) -> decltype(auto) {
		return std::visit([&arg, &second] (auto& obj) -> decltype(auto) { return obj.test_run(arg, second); }, variant);
	}
};


export
void runBenchmark(
    MyBenchmarkNS::BenchmarkParametr param,
    const size_t arraysCount,
    std::vector<AnyTest>& tests,
    const char* benchName
) noexcept {
    using namespace MyBenchmarkNS;

    auto generateVoidVectorsOfMemory = [&param] (size_t arraysCount) noexcept -> std::vector<Flat2DArray<uint8_t>> {
        vector<Flat2DArray<uint8_t>> res {}; res.reserve(arraysCount);

        for (size_t i = 0; i < arraysCount; i++) {
            res.push_back(Flat2DArray<uint8_t>(param.width, param.height, 32));
        }

        return res;
    };

    auto originalMemory = generateVectorOfTestMemory<uint8_t>(arraysCount, 32, param.width, param.height);
    auto secondArray = generateVoidVectorsOfMemory(arraysCount);

    printf("===== BENCHMARK: %s =====\n", benchName);
    const size_t _width = param.width;
    const size_t _height = param.height;
    printf("iterations count = %zu, arrays count = %zu, sizes: %zu * %zu, ", param.iterations, arraysCount, _width, _height);
    
    printf("warmup count: %zu\n", param.warmups);

    vector<BenchmarkResult> results; results.reserve(param.iterations);

    for (auto& test_variant : tests) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        string name = test_variant.getName();
        
        auto timings = MyBenchmarkNS::run(param.warmups, param.iterations, name, SaveTime::nanoseconds, [&] () {
            for (size_t iter = 0; iter < arraysCount; ++iter) {
                test_variant.run_sumAll(originalMemory[iter], secondArray[iter]);
            }
        });
        results.push_back(timings);
    }

    MyBenchmarkNS::printBenchmarkResults(results, MyBenchmarkNS::SaveTime::microseconds);
}