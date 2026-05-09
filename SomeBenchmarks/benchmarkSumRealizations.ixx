export module benchmarkSumRealizations;



import std;
import Flat2DArray;

import normalsum;
import normalsumv2;
import iter;
import sse_vertical;
import sse_horizontal;

import avx_horizontal;

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
};

export template<typename Fn, typename T>
void runBenchmark(
    std::vector<AnyTest>& tests,
    const std::vector<std::shared_ptr<Flat2DArray<T>>>& testData,
    const int iterations,
    Fn&& execute_test,
    const char* benchName
) noexcept {


    struct BenchmarkResult {
        std::string name;
        double avg_ms;   // Все значения хранятся в миллисекундах
        double stddev_ms;
        double min_ms;
        double max_ms;
        double p0_001_ms;  // 0.001%
        double p0_1_ms;    // 0.1%
        double p1_ms;      // 1%
        double p50_ms;     // Медиана
        double p99_ms;     // 99%
        double p99_9_ms;   // 99.9%
        double p99_999_ms; // 99.999%
    };

    auto formatTime = [] (double value, int precision = 7) noexcept -> string {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    };


    printf("===== BENCHMARK: %s =====\n", benchName);
    const size_t _width = testData[0]->width();
    const size_t _height = testData[0]->height();
    printf("iterations count = %d, arrays count = %u, sizes: %u * %u, ", iterations, (unsigned)testData.size(), (unsigned)_width, (unsigned)_height);
    
    const int warmupIterations = 3;
    printf("warmup count: %d\n", warmupIterations);
    std::vector<BenchmarkResult> results;
    results.reserve(tests.size());

    for (auto& test_variant : tests) {
        std::string name = test_variant.getName();
        std::vector<double> timings_ms; // Храним всё сразу в миллисекундах
        timings_ms.reserve(iterations * testData.size());

        // Прогревочные итерации
        for (int iter = 0; iter < warmupIterations; ++iter) {
            for (auto& vec : testData) {
                std::invoke(execute_test, test_variant, *vec);
            }
        }

        // Сбор данных
        for (int iter = 0; iter < iterations; ++iter) {
            for (auto& vec : testData) {
                auto start = std::chrono::high_resolution_clock::now();
                std::invoke(execute_test, test_variant, *vec);
                auto end = std::chrono::high_resolution_clock::now();

                // Конвертация НАПРЯМУЮ в миллисекунды
                auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(
                    end - start).count() / 1000.0; // микросекунды -> миллисекунды

                timings_ms.push_back(duration_ms);
            }
        }

        // Расчёт статистики
        if (timings_ms.empty()) continue;
        std::sort(timings_ms.begin(), timings_ms.end());
        const size_t n = timings_ms.size();
        const double total_ms = std::accumulate(timings_ms.begin(), timings_ms.end(), 0.0);

        auto get_percentile = [n] (const std::vector<double>& sorted, double p) {
            if (p <= 0.0) return sorted[0];
            if (p >= 1.0) return sorted[n - 1];
            double index = p * (n - 1);
            size_t lo = static_cast<size_t>(index);
            double fraction = index - lo;
            return (lo >= n - 1) ? sorted[n - 1] :
                sorted[lo] + fraction * (sorted[lo + 1] - sorted[lo]);
            };

        BenchmarkResult res;
        res.name = name;
        res.avg_ms = total_ms / n;
        res.min_ms = timings_ms[0];
        res.max_ms = timings_ms[n - 1];
        res.p0_001_ms = get_percentile(timings_ms, 0.00001);
        res.p0_1_ms = get_percentile(timings_ms, 0.001);
        res.p1_ms = get_percentile(timings_ms, 0.01);
        res.p50_ms = get_percentile(timings_ms, 0.5);
        res.p99_ms = get_percentile(timings_ms, 0.99);
        res.p99_9_ms = get_percentile(timings_ms, 0.999);
        res.p99_999_ms = get_percentile(timings_ms, 0.99999);

        // Стандартное отклонение
        double sum_sq_diff = 0.0;
        for (double t : timings_ms) {
            double diff = t - res.avg_ms;
            sum_sq_diff += diff * diff;
        }
        res.stddev_ms = std::sqrt(sum_sq_diff / n);

        results.push_back(res);
    }

    // Вывод итоговой таблицы В МИЛЛИСЕКУНДАХ
    if (!results.empty()) {
        const int precision = 5; // Знаков после запятой

        // Заголовок таблицы
        printf("\n[SUMMARY] Benchmark results (ALL TIMES IN MILLISECONDS)\n");
        printf("%-15s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s\n",
            "Algorithm", "Avg", "StdDev", "Min", "Max",
            "p0.001%", "p0.1%", "p1%", "p50%", "p99%", "p99.9%", "p99.999%");

        // Разделитель
        printf("%-15s-+-%s-+-%s-+-%s-+-%s-+-%s-+-%s-+-%s-+-%s-+-%s-+-%s-+-%s\n",
            std::string(15, '-').c_str(),
            "----------", "----------", "----------", "----------", "----------",
            "----------", "----------", "----------", "----------", "----------", "----------");

        // Данные таблицы
        for (const auto& r : results) {
            printf("%-15s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s\n",
                r.name.substr(0, 15).c_str(),
                formatTime(r.avg_ms, precision).c_str(),
                formatTime(r.stddev_ms, precision).c_str(),
                formatTime(r.min_ms, precision).c_str(),
                formatTime(r.max_ms, precision).c_str(),
                formatTime(r.p0_001_ms, precision).c_str(),
                formatTime(r.p0_1_ms, precision).c_str(),
                formatTime(r.p1_ms, precision).c_str(),
                formatTime(r.p50_ms, precision).c_str(),
                formatTime(r.p99_ms, precision).c_str(),
                formatTime(r.p99_9_ms, precision).c_str(),
                formatTime(r.p99_999_ms, precision).c_str()
            );
        }

        // Подпись таблицы
        printf("\nLegend:\n");
        printf("  All times in milliseconds (ms) - 1 ms = 0.001 seconds\n");
        printf("  pX%% = X-th percentile (time below which X%% of measurements fall)\n");
        printf("  Example: p99.9 = time below which 99.9%% of executions completed\n");
        printf("  StdDev = Standard deviation (stability indicator)\n");
    }
}

export template<typename T>
void runFullSumBenchmark(AnyTest& test_variant,
								Flat2DArray<T>& testData) {
	test_variant.run_sumAll(testData);
}