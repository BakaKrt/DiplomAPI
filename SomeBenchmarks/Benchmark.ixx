module;

#include <chrono>
#include <algorithm> // приходится инклудить, так как с обычным import'ом std IntelliSense нормально не работает
#include <numeric>

export module benchmark;

import std;

using std::string;
using std::vector;
using std::invoke;

using std::format;

export namespace MyBenchmarkNS {

    enum class SaveTime { None = 0, nanoseconds = 1, microseconds = 1000, milliseconds = 1000000};

    struct BenchmarkParametr {
        size_t warmups;
        size_t iterations;
        size_t width;
        size_t height;
    };

    struct BenchmarkResult {
        string name;
        double totalTime;   // Общее время выполнения
        double avg;         // Среднее время выполнения
        double stdDev;      // Стандартное отклонение
        double min;         // Минимальное время выполнения
        double max;         // Максимальное время выполнения
        double p0_01;       // 0.01%
        double p0_1;        // 0.1%
        double p1;          // 1%
        double p50;         // Медиана
        double p99;         // 99%
        double p99_9;       // 99.9%
        double p99_99;      // 99.99%
        double variation;   // Коэффициент вариации
        SaveTime savedAs;   // сохранён как

        BenchmarkResult() = default;

        BenchmarkResult operator +=(BenchmarkResult& other) {
            this->totalTime += other.totalTime;
            this->avg       += other.avg;
            this->stdDev    += other.stdDev;
            this->min       += other.min;
            this->max       += other.max;
            this->p0_01     += other.p0_01;
            this->p0_1      += other.p0_1;
            this->p1        += other.p1;
            this->p50       += other.p50;
            this->p99       += other.p99;
            this->p99_9     += other.p99_9;
            this->p99_99    += other.p99_99;
            this->variation += other.variation;
            return *this;
        }

        BenchmarkResult operator /(double del) {
            this->totalTime /= del;
            this->avg       /= del;
            this->stdDev    /= del;
            this->min       /= del;
            this->max       /= del;
            this->p0_01     /= del;
            this->p0_1      /= del;
            this->p1        /= del;
            this->p50       /= del;
            this->p99       /= del;
            this->p99_9     /= del;
            this->p99_99    /= del;
            this->variation /= del;
            return *this;
        }
    };

    double getScaleFactor(SaveTime unit) noexcept {
        return static_cast<double>(static_cast<int>(unit));
    }

    /// <summary>
    /// Возвращает суффикс единицы измерения
    /// </summary>
    /// <param name="unit"></param>
    /// <returns></returns>
    string getUnitSuffix(SaveTime unit) noexcept {
        switch (unit) {
            case SaveTime::nanoseconds: return "ns";
            case SaveTime::microseconds: return "us";
            case SaveTime::milliseconds: return "ms";
            default: return "?";
        }
    }

    /// <summary>
    /// Функция для форматирования числа в строку с заданной точностью. 
    /// Нужно для вычисления максимальной ширины колонки
    /// </summary>
    /// <param name="value"></param>
    /// <param name="scale"></param>
    /// <param name="precision"></param>
    /// <returns></returns>
    string formatDouble(double value, double scale, int precision = 2) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << (value / scale);
        return oss.str();
    }

    void printBenchmarkResults(vector<BenchmarkResult>& results, SaveTime printAs) noexcept {
        if (results.empty()) {
            std::cout << "No benchmark results to display." << std::endl;
            return;
        }

        const double scale = getScaleFactor(printAs);
        const std::string suffix = getUnitSuffix(printAs);

        // Точность вывода для времени (количество знаков после запятой)
        constexpr int timePrecision = 3;
        // Точность для variation (коэффициент вариации, обычно безразмерный или %)
        constexpr int varPrecision = 2;

        // 1. Вычисляем максимальную ширину для колонки имени
        size_t maxNameWidth = 4; // Минимальная ширина для заголовка "Name"
        for (const auto& res : results) {
            maxNameWidth = std::max(maxNameWidth, res.name.length());
        }

        // 2. Вычисляем максимальную ширину для числовых колонок
        // Нам нужно пройтись по всем данным, превратить их в строки и найти самую длинную
        size_t maxNumWidth = 0;

        auto updateMaxWidth = [&] (double val) {
            std::string s = formatDouble(val, scale, timePrecision);
            if (s.length() > maxNumWidth) maxNumWidth = s.length();
        };

        auto updateVarWidth = [&] (double val) {
            std::string s = formatDouble(val, 1.0, varPrecision); // Variation не масштабируем
            if (s.length() > maxNumWidth) maxNumWidth = s.length();
        };

        for (const auto& res : results) {
            //updateMaxWidth(res.totalTime);
            updateMaxWidth(res.avg);
            updateMaxWidth(res.stdDev);
            updateMaxWidth(res.min);
            updateMaxWidth(res.max);
            updateMaxWidth(res.p0_01);
            updateMaxWidth(res.p0_1);
            updateMaxWidth(res.p1);
            updateMaxWidth(res.p50);
            updateMaxWidth(res.p99);
            updateMaxWidth(res.p99_9);
            updateMaxWidth(res.p99_99);
            updateVarWidth(res.variation);
        }

        // Добавляем немного отступа к ширине чисел, чтобы не слипалось
        maxNumWidth = std::max(maxNumWidth, static_cast<size_t>(6)); // Минимальная ширина для числа "0.00"

        // 3. Формируем разделитель и заголовки
        // Helper для создания строки-разделителя
        auto makeSeparator = [] (size_t width, char ch = '-') {
            return std::string(width, ch);
            };

        std::string nameHeader = "Name";
        std::string numHeader = "Value [" + suffix + "]";
        std::string varHeader = "Var [%]"; // Или просто "CV"

        // Корректируем заголовки, если они уже чем-то заполнены, но для простой таблицы 
        // часто делают одну ширину для всех числовых колонок или разные.
        // Для компактности сделаем все числовые колонки одной ширины (maxNumWidth),
        // кроме, возможно, TotalTime, если он сильно выбивается, но для унификации оставим одинаково.

        // Однако, таблица с 13 числовыми колонками будет очень широкой.
        // Часто в бенчмарках выводят только Avg, Min, Max, P50, P99.
        // Но раз структура полная, выведем всё.

        // Чтобы таблица влезала в консоль, иногда уменьшают точность или сокращают названия.
        // Здесь реализуем полный вывод с динамической шириной.

        const size_t colWidth = maxNumWidth + 1; // +1 для минимального пробела внутри ячейки при выравнивании

        // Функция печати одной строки данных
        auto printRow = [&] (const BenchmarkResult& res) {
            std::cout << std::left << std::setw(maxNameWidth) << res.name << " | ";

            // Lambda для печати числа с правильной шириной
            auto printVal = [&] (double val) {
                std::cout << std::right << std::setw(colWidth)
                    << formatDouble(val, scale, timePrecision) << " | ";
            };

            auto printVar = [&] (double val) {
                std::cout << std::right << std::setw(colWidth)
                    << formatDouble(val, 1.0, varPrecision) << " | ";
            };

            //printVal(res.totalTime);
            printVal(res.avg);
            printVal(res.stdDev);
            printVal(res.min);
            printVal(res.max);
            printVal(res.p0_01);
            printVal(res.p0_1);
            printVal(res.p1);
            printVal(res.p50);
            printVal(res.p99);
            printVal(res.p99_9);
            printVal(res.p99_99);
            printVar(res.variation);

            std::cout << "\n";
        };

        // --- Вывод ---

        // Заголовок таблицы
        std::cout << std::left << std::setw(maxNameWidth) << nameHeader << " | ";

        // Генерируем заголовки для колонок. 
        // Чтобы не загромождать, можно использовать сокращения, соответствующие полям структуры
        std::vector<std::string> headers = {
            /*"Total", */"Avg", "StdDev", "Min", "Max",
            "0.01%", "0.1%", "1%", "50%", "99%", "99.9%", "99.99%", "CV %"
        };

        for (const auto& h : headers) {
            // Если заголовок короче ширины колонки, центрируем или выравниваем
            std::cout << std::right << std::setw(colWidth) << h << " | ";
        }
        std::cout << "\n";

        // Линия разделителя
        size_t totalWidth = maxNameWidth + 1 + (headers.size() * (colWidth + 3)); // 3 = " | "
        std::cout << std::string(totalWidth, '-') << "\n";

        // Данные
        for (const auto& res : results) {
            printRow(res);
        }

        std::cout << std::endl;
    }

    /// <summary>
    /// Выполняет "прогрев" кода
    /// </summary>
    /// <typeparam name="Callable"></typeparam>
    /// <param name="warmups"></param>
    /// <param name="codeToBenchmark"></param>
    template<typename Callable>
    void warmup(size_t warmups, Callable&& codeToBenchmark) {
        for (size_t i = 0; i < warmups; ++i) {
            invoke(codeToBenchmark);
        }
    }

    
    template<typename Callable>
    BenchmarkResult run(size_t warmups, size_t runCount, string name, SaveTime saveAs, Callable&& codeToBenchmark) {
        using namespace std::chrono;

        vector<double> timings {}; timings.reserve(runCount);

        // прогрев
        warmup(warmups, codeToBenchmark);

        steady_clock::time_point start, end;
        double duration;

        // замер времени выполнения
        for (size_t currentRunCount = 0; currentRunCount < runCount; currentRunCount++) {
            start = high_resolution_clock::now();
            invoke(codeToBenchmark);
            end = high_resolution_clock::now();

            duration = duration_cast<nanoseconds>(end - start).count() / (double) saveAs;
            timings.push_back(duration);
        }

        // сортировка времени выполнения по возврастанию
        if (timings.empty()) return BenchmarkResult {};
        std::sort(timings.begin(), timings.end());

        const size_t timingsCount = timings.size();
        const double totalMs = std::accumulate(timings.begin(), timings.end(), 0.0);

        auto getPercentile = [timingsCount] (const vector<double>& sorted, double percentage) {
            if (percentage <= 0.0) return sorted[0];
            if (percentage >= 1.0) return sorted[timingsCount - 1];
            double index = percentage * (timingsCount - 1);
            size_t lo = static_cast<size_t>(index);
            double fraction = index - lo;
            return (lo >= timingsCount - 1) ? sorted[timingsCount - 1] :
                sorted[lo] + fraction * (sorted[lo + 1] - sorted[lo]);
        };

        BenchmarkResult result {};

        result.totalTime = totalMs;
        result.avg = totalMs / timingsCount;

        result.name = name;

        result.min = timings[0];
        result.max = timings[timingsCount - 1];

        result.p0_01    = getPercentile(timings, 0.0001);
        result.p0_1     = getPercentile(timings, 0.001);
        result.p1       = getPercentile(timings, 0.01);
        result.p50      = getPercentile(timings, 0.5);
        result.p99      = getPercentile(timings, 0.99);
        result.p99_9    = getPercentile(timings, 0.999);
        result.p99_99   = getPercentile(timings, 0.9999);

        // стандартное отклонение
        double sumSqDiff = 0.0;
        for (double timing : timings) {
            double diff = timing - result.avg;
            sumSqDiff += diff * diff;
        }

        result.stdDev = std::sqrt(sumSqDiff / timingsCount);
        result.variation = result.stdDev * 100 / result.avg;
        result.savedAs = saveAs;

        return result;
    }
}