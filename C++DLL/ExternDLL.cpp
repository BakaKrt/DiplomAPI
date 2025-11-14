#include <iomanip>


#include "ExternDLL.h"

#include "ForBenchmarks/CV_base.h"
#include "ForBenchmarks/CV_base_alt_sum.h"
#include "ForBenchmarks/CV_mt_wMTc.h"
#include "ForBenchmarks/CV_mt_woMTc.h"
#include "ForBenchmarks/CV_mt_wMTc_alt_sum.h"

using namespace std::chrono;
using std::unique_ptr;
using std::make_unique;
using std::cout;

extern "C" {
#pragma region HeightMap

    /// <summary>
    /// Создать карту высот
    /// </summary>
    /// <param name="width">Ширина</param>
    /// <param name="height">Высота</param>
    /// <param name="setRandomValue">Заполнить случайными значениями, по умолчанию true</param>
    /// <returns>Указатель на карту высот</returns>
    DLL_EXPORT HeightMap* CreateHeightMap(size_t width, size_t height, bool setRandomValue) {
        try {
            return new HeightMap(width, height, setRandomValue);
        }
        catch (...) {
            return nullptr;
        }
    }

    /// <summary>
    /// Создать карту высот
    /// </summary>
    /// <param name="width">Ширина</param>
    /// <param name="height">Высота</param>
    /// <param name="setRandomValue">Заполнить случайными значениями, по умолчанию true</param>
    ///// <returns>Указатель на карту высот</returns>
    //DLL_EXPORT HeightMap* HeightMap_Create(size_t width, size_t height, bool setRandomValue)
    //{
    //    try {
    //        return new HeightMap(width, height, setRandomValue);
    //    }
    //    catch (...) {
    //        return nullptr;
    //    }
    //}

    /// <summary>
    /// Создать карту высот
    /// </summary>
    /// <param name="width">Ширина</param>
    /// <param name="height">Высота</param>
    /// <param name="threadCount">Количество потоков</param>
    /// <param name="setRandomValue">Заполнить случайными значениями</param>
    /// <returns>Указатель на карту высот</returns>
    DLL_EXPORT HeightMap* HeightMap_Create(size_t width, size_t height, int threadCount, bool setRandomValue) {
        try {
            return new HeightMap(width, height, threadCount, setRandomValue);
        }
        catch (...) {
            return nullptr;
        }
    }

    /// <summary>
    /// Установка правил
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="rules">Массив булевых значений длиной 8</param>
    DLL_EXPORT void HeightMap_SetRules(HeightMap* obj, const bool rules[8]) {
        if (obj && rules) {
            obj->SetRules(rules);
        }
    }

    /// <summary>
    /// Установка правил
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="r1">Верхний правый угол</param>
    /// <param name="r2">Верх</param>
    /// <param name="r3">Верхний левый угол</param>
    /// <param name="r4">Лево</param>
    /// <param name="r5">Право</param>
    /// <param name="r6">Левый нижний угол</param>
    /// <param name="r7">Низ</param>
    /// <param name="r8">Правый нижний угол</param>
    DLL_EXPORT void HeightMap_SetRulesEx(HeightMap* obj,
        bool r1, bool r2, bool r3, bool r4,
        bool r5, bool r6, bool r7, bool r8)
    {

        if (obj) {
            obj->SetRules(r1, r2, r3, r4, r5, r6, r7, r8);
        }
    }

    /// <summary>
    /// Вызвать однопоточный Tick для карты высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="count">Количество тиков, по умолчанию 1</param>
    DLL_EXPORT void HeightMap_Tick(HeightMap* obj, size_t count) {
        if (obj) {
            obj->Tick(count);
        }
    }

    /// <summary>
    /// Вызвать многопоточный Tick для карты высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="count">Количество тиков, по умолчанию 1</param>
    DLL_EXPORT void HeightMap_TickMT(HeightMap* obj, int count) {
        if (obj) {
            obj->TickMT(count);
        }
    }

    /// <summary>
    /// Узнать количество потоков
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t HeightMap_GetThreadCount(HeightMap* obj) {
        if (obj) return obj->GetThreadsCount();
        return -1;
    }

    /// <summary>
    /// Получить указатель на матрицу
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <returns>Указатель на матрицу</returns>
    DLL_EXPORT Flat2DByte* HeightMap_GetMatrix(HeightMap* obj) {
        if (obj) return obj->GetMatrix();
        return nullptr;
    }

    /// <summary>
    /// Получить ширину карты высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <returns>Ширина карты высот</returns>
    DLL_EXPORT size_t HeightMap_GetWidth(HeightMap* obj) {
        if (obj) return obj->GetWidth();
        return 0;
    }

    /// <summary>
    /// Получить высоту карты высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <returns>Высота карты высот</returns>
    DLL_EXPORT size_t HeightMap_GetHeight(HeightMap* obj) {
        if (obj) return obj->GetHeight();
        return 0;
    }

    /// <summary>
    /// Нормализация карты высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <returns></returns>
    DLL_EXPORT void HeightMap_Normalize(HeightMap* obj) {
        if (obj) {
            obj->Normalize();
        }
    }

    /// <summary>
    /// Кнопка сделать хорошо
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="type">Тип</param>
    DLL_EXPORT void HeightMap_MakeGood(HeightMap* obj, int type) {
        if (obj) {
            obj->MakeGood(type);
        }
    }

    /// <summary>
    /// Уничтожить карту высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    DLL_EXPORT void DestroyHeightMap(HeightMap* obj) {
        delete obj;
    }

    /// <summary>
    /// Уничтожить карту высот
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    DLL_EXPORT void HeightMap_Destroy(HeightMap* obj) {
        delete obj;
    }

#pragma endregion

#pragma region Flat2DByte

    DLL_EXPORT Flat2DByte* CreateFlat2DByte(size_t width, size_t height) {
        try {
            return new Flat2DByte(width, height);
        }
        catch (...) {
            return nullptr;
        }
    }


    DLL_EXPORT void Flat2DByte_SetData(Flat2DByte* obj, size_t x, size_t y, byte value) {
        if (obj) {
            try {
                obj->at(x, y) = value;
            }
            catch (const std::out_of_range&) {

            }
        }
    }


    DLL_EXPORT byte Flat2DByte_GetData(Flat2DByte* obj, size_t x, size_t y) {
        if (obj) {
            try {
                return obj->at(x, y);
            }
            catch (const std::out_of_range&) {
                return 0;
            }
        }
        return 0;
    }


    DLL_EXPORT byte* Flat2DByte_GetDataPtr(Flat2DByte* obj) {
        if (obj) {
            return obj->data();
        }
        return nullptr;
    }


    DLL_EXPORT size_t Flat2DByte_GetDataSize(Flat2DByte* obj) {
        if (obj) {
            return obj->capacity();
        }
        return 0;
    }


    DLL_EXPORT size_t Flat2DByte_GetWidth(Flat2DByte* obj) {
        if (obj) {
            return obj->width();
        }
        return 0;
    }


    DLL_EXPORT size_t Flat2DByte_GetHeight(Flat2DByte* obj) {
        if (obj) {
            return obj->height();
        }
        return 0;
    }


    DLL_EXPORT void DestroyFlat2DByte(Flat2DByte* obj) {
        delete obj;
    }
#pragma endregion

#pragma region Flat2DFloat

    DLL_EXPORT Flat2DFloat* CreateFlat2DFloat(size_t width, size_t height) {
        try {
            return new Flat2DFloat(width, height);
        }
        catch (...) {
            return nullptr;
        }
    }


    DLL_EXPORT void Flat2DFloat_SetData(Flat2DFloat* obj, size_t x, size_t y, byte value) {
        if (obj) {
            try {
                obj->at(x, y) = value;
            }
            catch (const std::out_of_range&) {

            }
        }
    }


    DLL_EXPORT float Flat2DFloat_GetData(Flat2DFloat* obj, size_t x, size_t y) {
        if (obj) {
            try {
                return obj->at(x, y);
            }
            catch (const std::out_of_range&) {
                return 0;
            }
        }
        return 0;
    }


    DLL_EXPORT float* Flat2DFloat_GetDataPtr(Flat2DFloat* obj) {
        if (obj) {
            return obj->data();
        }
        return nullptr;
    }


    DLL_EXPORT size_t Flat2DFloat_GetDataSize(Flat2DFloat* obj) {
        if (obj) {
            return obj->capacity();
        }
        return 0;
    }


    DLL_EXPORT size_t Flat2DFloat_GetWidth(Flat2DFloat* obj) {
        if (obj) {
            return obj->width();
        }
        return 0;
    }


    DLL_EXPORT size_t Flat2DFloat_GetHeight(Flat2DFloat* obj) {
        if (obj) {
            return obj->height();
        }
        return 0;
    }


    DLL_EXPORT void DestroyFlat2DFloat(Flat2DFloat* obj) {
        delete obj;
    }
#pragma endregion
}

static void BenchmarkHeightMapOTMT(size_t benchmarkIterations, size_t operationsPerIteration, size_t width, size_t height, int threadCount = 2) {
    if (operationsPerIteration == 0) {
        cout << "Error: operationsPerIteration must be greater than 0.\n";
        return;
    }

    cout << "benchmarkIterations: " << benchmarkIterations << " operationsPerIteration: "
        << operationsPerIteration << "\nwidth: " << width << " height: " << height << " threadCount: " << threadCount << '\n';

    duration<double> totalSyncTime = duration<double>(0.0);
    duration<double> totalAsyncTime = duration<double>(0.0);

    steady_clock::time_point start;
    steady_clock::time_point end;

    for (size_t x = 0; x < benchmarkIterations; x++) {
        HeightMap sync_map = HeightMap(width, height, threadCount, true);
        HeightMap async_map = sync_map; // Копируем из sync_map

        // --- Измеряем асинхронную версию ---
        start = steady_clock::now();
        async_map.TickMT(operationsPerIteration);
        end = steady_clock::now();
        totalAsyncTime += end - start;
        //cout << "totalAsyncTime" << totalAsyncTime.count() << "\n";

        // --- Измеряем синхронную версию ---
        start = steady_clock::now();
        sync_map.Tick(operationsPerIteration);
        end = steady_clock::now();
        totalSyncTime += end - start;
        //cout << "totalSyncTime" << totalSyncTime.count() << "\n";
    }

    // Вычисляем среднее время на одну операцию (Tick или TickAsync)
    double avgSyncTimeMs = (totalSyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;
    double avgAsyncTimeMs = (totalAsyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;

    // Выводим результаты
    cout << "Benchmark Results:\n";
    cout << "Total Sync Time: " << totalSyncTime.count() << " seconds\n";
    cout << "Total Async Time: " << totalAsyncTime.count() << " seconds\n";
    cout << "Average Sync Time per operation: " << avgSyncTimeMs << " ms\n";
    cout << "Average Async Time per operation: " << avgAsyncTimeMs << " ms\n";

    // Выводим коэффициент ускорения, если асинхронный метод быстрее (и делаем проверку на 0)
    if (avgSyncTimeMs > 0.0) {
        if (avgAsyncTimeMs > 0.0) {
            double speedup = avgSyncTimeMs / avgAsyncTimeMs;
            cout << "Speedup (Sync/Async): " << speedup << "x\n";
        }
        else {
            cout << "Async time is 0, cannot calculate speedup.\n";
        }
    }
    else {
        cout << "Sync time is 0, cannot calculate speedup.\n";
    }
}

static void BenchmarkCaveOTMT(size_t benchmarkIterations, size_t operationsPerIteration, size_t width, size_t height, int threadCount = 2) {
    if (operationsPerIteration == 0) {
        cout << "Error: operationsPerIteration must be greater than 0.\n";
        return;
    }

    cout << "benchmarkIterations: " << benchmarkIterations << " operationsPerIteration: "
        << operationsPerIteration << "\nwidth: " << width << " height: " << height << " threadCount: " << threadCount << '\n';

    duration<double> totalSyncTime = duration<double>(0.0f);
    duration<double> totalAsyncTime = duration<double>(0.0f);

    steady_clock::time_point start;
    steady_clock::time_point end;

    for (size_t x = 0; x < benchmarkIterations; x++) {
        // Создаём исходное состояние для этой итерации бенчмарка
        CaveGenerator* base_cave = new CaveGenerator(width, height, threadCount, true);
        base_cave->SetB({3});
        base_cave->SetB({2, 3});


        // --- Измеряем асинхронную версию ---
        {
            CaveGenerator* async_cave = new CaveGenerator(*base_cave); // Создаём копию из base_cave
            start = steady_clock::now();
            async_cave->TickMT(operationsPerIteration);
            end = steady_clock::now();
            totalAsyncTime += end - start;
            delete async_cave; // Удаляем копию сразу после замера
        } // Конец блока для асинхронной версии

        // --- Измеряем синхронную версию ---
        {
            CaveGenerator* sync_cave = new CaveGenerator(*base_cave); // Создаём новую копию из base_cave
            start = steady_clock::now();
            sync_cave->Tick(operationsPerIteration);
            end = steady_clock::now();
            totalSyncTime += end - start;
            delete sync_cave; // Удаляем копию сразу после замера
        } // Конец блока для синхронной версии

        delete base_cave; // Удаляем исходное состояние
    }

    // Вычисляем среднее время на одну операцию (Tick или TickAsync)
    double avgSyncTimeMs = (totalSyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;
    double avgAsyncTimeMs = (totalAsyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;

    // Выводим результаты
    cout << "Benchmark Results:\n";
    cout << "Total Sync Time: " << totalSyncTime.count() << " seconds\n";
    cout << "Total Async Time: " << totalAsyncTime.count() << " seconds\n";
    cout << "Average Sync Time per operation: " << avgSyncTimeMs << " ms\n";
    cout << "Average Async Time per operation: " << avgAsyncTimeMs << " ms\n";

    // Выводим коэффициент ускорения, если асинхронный метод быстрее (и делаем проверку на 0)
    if (avgSyncTimeMs > 0.0) {
        if (avgAsyncTimeMs > 0.0) {
            double speedup = avgSyncTimeMs / avgAsyncTimeMs;
            cout << "Speedup (Sync/Async): " << speedup << "x\n";
        }
        else {
            cout << "Async time is 0, cannot calculate speedup.\n";
        }
    }
    else {
        cout << "Sync time is 0, cannot calculate speedup.\n";
    }
}

static void BenchmarkCaveThreadsSpeedUp(int benchmarkIterations, int operationsPerIteration, size_t width, size_t height, int threadCountFrom = 2, int threadCountTo = 0) {
    if (operationsPerIteration == 0) {
        cout << "Error: operationsPerIteration must be greater than 0.\n";
        return;
    }

    cout << "benchmarkIterations: " << benchmarkIterations << " operationsPerIteration: "
        << operationsPerIteration << "\nwidth: " << width << " height: " << height << " threadCount: " << threadCountFrom << '\n';
    
    threadCountTo = GetThreadsCount(threadCountTo);

    std::vector<duration<double>> times = {};
    times.reserve(threadCountTo);

    steady_clock::time_point start;
    steady_clock::time_point end;

    for (int threads = threadCountFrom; threads <= threadCountTo; threads++) {

        duration<double> thisTime = duration<double>(0.0f);

        for (auto iterations = 0; iterations < benchmarkIterations; iterations++) {
            CaveGenerator* cave = new CaveGenerator(width, height, threads, true);
            start = steady_clock::now();
            cave->TickMT(operationsPerIteration);
            end = steady_clock::now();

            thisTime += end - start;

            delete cave;
        }
        times.push_back(thisTime);
        cout << "threads: " << threads << " time:" << thisTime.count() << std::endl;
        thisTime = duration<double>(0.0f);
    }

    for (const auto& time : times) {
        cout << "Threads: " << threadCountFrom << "\t";
        cout << "Time: " << time.count() << std::endl;
        threadCountFrom++;
    }
}

//struct BenchmarkResult {
//    duration<double> elapsed_time;
//    std::string implementation_name;
//    std::string test_type; // "Single-threaded" или "Multi-threaded"
//    int iterations;
//};
//
//// Функция для запуска однопоточного теста (только для base)
//static BenchmarkResult RunSingleThreadedBenchmark(const CaveGenerator_base& original_cave, int iterations, int ops_per_iteration) {
//    //auto cave_copy = std::make_unique<CaveGenerator_base>(original_cave);
//
//    steady_clock::duration totalTime {};
//
//    for (int i = 0; i < iterations; ++i) {
//        auto cave_copy = new CaveGenerator_base(original_cave);
//
//        auto start = steady_clock::now();
//            cave_copy->Tick(ops_per_iteration);
//        auto end = steady_clock::now();
//        totalTime += end - start;
//    }
//
//    return {  totalTime, "CaveGenerator_base", "Single-threaded (Tick)", iterations };
//}
//
//// Функция для запуска многопоточного теста
//template<typename T>
//BenchmarkResult RunMultiThreadedBenchmark(const T& original_cave, int iterations, int ops_per_iteration) {
//    //auto cave_copy = std::make_unique<CaveGenerator_base>(original_cave);
//
//    steady_clock::duration totalTime{};
//
//    for (int i = 0; i < iterations; ++i) {
//        auto cave_copy = new T (original_cave);
//
//        auto start = steady_clock::now();
//            cave_copy->TickMT(ops_per_iteration);
//        auto end = steady_clock::now();
//        totalTime += end - start;
//    }
//
//    std::string readable_name;
//    if (std::is_same_v<T, CaveGenerator_base>) {
//        readable_name = "CaveGenerator_base_MT";
//    }
//    else if (std::is_same_v<T, CaveGenerator_mt_wMTcalc>) {
//        readable_name = "CaveGenerator_mt_wMTcalc";
//    }
//    else if (std::is_same_v<T, CaveGenerator_mt_woMTcalc>) {
//        readable_name = "CaveGenerator_mt_woMTcalc";
//    }
//    else if (std::is_same_v<T, CaveGenerator_mt_wMTcalc_alt_sum>) {
//        readable_name = "CaveGenerator_mt_woMTcalc_alt_sum";
//    }
//    else {
//        readable_name = "Unknown_MT_Impl";
//    }
//
//    return { totalTime, readable_name, "Multi-threaded (TickMT)", iterations };
//}
//
//static void test(int benchmarkIterations, int operationsPerIteration, size_t width, size_t height, int threadsCount) {
//    std::cout << "\n--- Running Comprehensive Benchmark ---\n";
//    std::cout << "Parameters: Width=" << width << ", Height=" << height
//        << ", Threads=" << threadsCount << ", Iterations=" << benchmarkIterations
//        << ", OpsPerIter=" << operationsPerIteration << "\n";
//
//    auto original_base = std::make_unique<CaveGenerator_base>(width, height, threadsCount, true);
//    auto original_wMT = std::make_unique<CaveGenerator_mt_wMTcalc>(*original_base);
//    auto original_woMT = std::make_unique<CaveGenerator_mt_woMTcalc>(*original_base);
//    auto original_woMT_alt = std::make_unique<CaveGenerator_mt_wMTcalc_alt_sum>(*original_base);
//
//    std::vector<BenchmarkResult> results;
//
//    // 1. Тестирование CaveGenerator_base: однопоточный (Tick)
//    std::cout << "Running single-threaded test for CaveGenerator_base...\n";
//    results.push_back(RunSingleThreadedBenchmark(*original_base, benchmarkIterations, operationsPerIteration));
//
//    // 2. Тестирование CaveGenerator_base: многопоточный (TickMT)
//    //std::cout << "Running multi-threaded test for CaveGenerator_base...\n";
//    //results.push_back(RunMultiThreadedBenchmark<CaveGenerator_base>(*original_base, benchmarkIterations, operationsPerIteration));
//
//    // 3. Тестирование CaveGenerator_mt_wMTcalc: многопоточный (TickMT)
//    std::cout << "Running multi-threaded test for CaveGenerator_mt_wMTcalc...\n";
//    results.push_back(RunMultiThreadedBenchmark<CaveGenerator_mt_wMTcalc>(*original_wMT, benchmarkIterations, operationsPerIteration));
//
//    // 4. Тестирование CaveGenerator_mt_woMTcalc: многопоточный (TickMT)
//    std::cout << "Running multi-threaded test for CaveGenerator_mt_woMTcalc...\n";
//    results.push_back(RunMultiThreadedBenchmark<CaveGenerator_mt_woMTcalc>(*original_woMT, benchmarkIterations, operationsPerIteration));
//
//    // 5. Тестирование CaveGenerator_mt_woMTcalc_alt: многопоточный (TickMT)
//    std::cout << "Running multi-threaded test for CaveGenerator_mt_woMTcalc_sum_alt...\n";
//    results.push_back(RunMultiThreadedBenchmark<CaveGenerator_mt_wMTcalc_alt_sum>(*original_woMT_alt, benchmarkIterations, operationsPerIteration));
//
//    // Вывод результатов
//    std::cout << "\n--- Benchmark Results ---\n";
//    for (const auto& res : results) {
//        double total_time = res.elapsed_time.count();
//        double time_per_iteration = total_time / static_cast<double>(res.iterations);
//
//        std::cout << std::fixed << std::setprecision(8);
//        std::cout << res.implementation_name << " (" << res.test_type << "):\n";
//        std::cout << "\tTotal Time: " << total_time << " seconds\n";
//        std::cout << "\tTime Per Iteration: " << time_per_iteration << " seconds\n";
//    }
//
//    // Сравнение
//    if (results.size() >= 2) {
//        const auto& base_st = results[0]; // Однопоточная base - эталон
//
//        std::cout << std::fixed << std::setprecision(8);
//
//        // Сравнение Base_MT с Base_ST
//        if (base_st.elapsed_time.count() > 0.0) {
//            double speedup = base_st.elapsed_time.count() / results[1].elapsed_time.count();
//            std::cout << "\nSpeedup (Base_ST / Base_MT): " << speedup << "x\n";
//        }
//        else {
//            std::cout << "\nSpeedup calculation (Base_ST / Base_MT) skipped (ST time is 0).\n";
//        }
//
//        // Сравнение всех остальных реализаций (начиная с индекса 1) с Base_ST
//        for (size_t i = 1; i < results.size(); ++i) { // Начинаем с 1, т.к. 0 - это Base_ST
//            const auto& other_result = results[i];
//            //if (base_st.elapsed_time.count() > 0.0) {
//                double speedup = base_st.elapsed_time.count() / other_result.elapsed_time.count();
//                std::cout << "Speedup (Base_ST / " << other_result.implementation_name << "): " << speedup << "x\n";
//            //}
//            //else {
//                //std::cout << "Speedup calculation (Base_ST / " << other_result.implementation_name << ") skipped (ST time is 0).\n";
//            //}
//        }
//    }
//
//    std::cout << "------------------------\n";
//}


// Структура для хранения результата бенчмарка ---
struct BenchmarkResult {
    duration<double> elapsed_time;
    std::string implementation_name;
    std::string test_type; // "Single-threaded" или "Multi-threaded"
    int iterations;
};

// Вспомогательная функция для замера времени ---
template<typename Func>
duration<double> measureTime(Func&& func) {
    auto start = steady_clock::now();
    func();
    auto end = steady_clock::now();
    return end - start;
}

// Основная функция бенчмарка ---
static void BenchmarkCaveImplementations(size_t benchmarkIterations, size_t operationsPerIteration, size_t width, size_t height, int threadCount = 2) {
    if (operationsPerIteration == 0) {
        cout << "Error: operationsPerIteration must be greater than 0.\n";
        return;
    }

    cout << "=== Benchmark Configuration ===\n";
    cout << "benchmarkIterations: " << benchmarkIterations << "\n";
    cout << "operationsPerIteration: " << operationsPerIteration << "\n";
    cout << "width: " << width << ", height: " << height << "\n";
    cout << "threadCount: " << threadCount << "\n";
    cout << "===============================\n\n";

    // Создаём эталонное начальное состояние
    auto base_cave = make_unique<CaveGenerator_base>(width, height, threadCount, true);
    // Установим одинаковые правила для всех, если это важно
    base_cave->SetB({ 3 }); // Пример
    base_cave->SetS({ 2, 3 }); // Пример

    vector<BenchmarkResult> results;

    // --- 1. Базовая однопоточная реализация ---
    cout << "Benchmarking Base (Single-threaded)...\n";
    duration<double> totalBaseSyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_base>(*base_cave);
        totalBaseSyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->Tick(operationsPerIteration);
            });
    }
    results.push_back({ totalBaseSyncTime, "CaveGenerator_base (Tick)", "Single-threaded", static_cast<int>(benchmarkIterations) });

    // --- 1.1. Альт однопоточная реализация ---
    cout << "Benchmarking Base Alt (Single-threaded)...\n";
    duration<double> totalBaseAltSyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_base_alt_calc>(*base_cave);
        totalBaseAltSyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->Tick(operationsPerIteration);
            });
    }
    results.push_back({ totalBaseAltSyncTime, "CaveGenerator_base Alt (Tick)", "Single-threaded", static_cast<int>(benchmarkIterations) });

    // --- 2. Базовая многопоточная реализация ---
    cout << "Benchmarking Base (Multi-threaded)...\n";
    duration<double> totalBaseAsyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_base>(*base_cave);
        totalBaseAsyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->TickMT(operationsPerIteration);
            });
    }
    results.push_back({ totalBaseAsyncTime, "CaveGenerator_base (TickMT)", "Multi-threaded", static_cast<int>(benchmarkIterations) });

    // --- 2.1. Базовая многопоточная реализация ---
    cout << "Benchmarking Base Alt (Multi-threaded)...\n";
    duration<double> totalBaseAltAsyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_base_alt_calc>(*base_cave);
        totalBaseAltAsyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->TickMT(operationsPerIteration);
            });
    }
    results.push_back({ totalBaseAltAsyncTime, "CaveGenerator_base Alt (TickMT)", "Multi-threaded", static_cast<int>(benchmarkIterations) });

    // --- 3. Наследник: CaveGenerator_mt_wMTcalc_alt_sum (только многопоток) ---
    cout << "Benchmarking mt_wMTcalc_alt_sum (Multi-threaded)...\n";
    duration<double> totalMTcalcAltSumAsyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_mt_wMTcalc_alt_sum>(*base_cave);
        totalMTcalcAltSumAsyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->TickMT(operationsPerIteration);
            });
    }
    results.push_back({ totalMTcalcAltSumAsyncTime, "CaveGenerator_mt_wMTcalc_alt_sum (TickMT)", "Multi-threaded", static_cast<int>(benchmarkIterations) });
    
    // --- 4. Наследник: CaveGenerator_mt_wMTc (только многопоток) ---
    cout << "Benchmarking mt_wMTcalc (Multi-threaded)...\n";
    duration<double> totalMTcalcAsyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_mt_wMTcalc>(*base_cave);
        totalMTcalcAsyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->TickMT(operationsPerIteration);
            });
    }
    results.push_back({ totalMTcalcAsyncTime, "CaveGenerator_mt_wMTcalc (TickMT)", "Multi-threaded", static_cast<int>(benchmarkIterations) });

    // --- 5. Наследник: CaveGenerator_mt_woMTcalc (только многопоток) ---
    cout << "Benchmarking mt_woMTcalc (Multi-threaded)...\n";
    duration<double> totalMTwoCalcAsyncTime = duration<double>::zero();
    for (size_t i = 0; i < benchmarkIterations; ++i) {
        auto cave_copy = make_unique<CaveGenerator_mt_woMTcalc>(*base_cave);
        totalMTwoCalcAsyncTime += measureTime([&cave_copy, operationsPerIteration]() {
            cave_copy->TickMT(operationsPerIteration);
            });
    }
    results.push_back({ totalMTwoCalcAsyncTime, "CaveGenerator_mt_woMTcalc (TickMT)", "Multi-threaded", static_cast<int>(benchmarkIterations) });

    // --- Вывод результатов ---
    cout << "\n=== Benchmark Results ===\n";
    for (const auto& res : results) {
        double avgTimeMs = (res.elapsed_time.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;
        cout << "Implementation: " << res.implementation_name << "\n";
        cout << "  Type: " << res.test_type << "\n";
        cout << "  Total Time: " << res.elapsed_time.count() << " seconds\n";
        cout << "  Average Time per operation: " << avgTimeMs << " ms\n\n";
    }

    // --- Сравнение с базовой однопоточной версией ---
    cout << "=== Performance vs Base Single-threaded ===\n";
    duration<double> baseSyncTime = results[0].elapsed_time;
    if (baseSyncTime.count() > 0.0) {
        for (size_t i = 1; i < results.size(); ++i) {
            const auto& res = results[i];
            double speedup = baseSyncTime.count() / res.elapsed_time.count();
            cout << "Speedup (" << results[0].implementation_name << " / " << res.implementation_name << "): " << speedup << "x\n";
        }
    }
    else {
        cout << "Base sync time is 0, cannot calculate speedup.\n";
    }
    cout << "========================\n";
}


int main()
{
    using std::cout;

    BenchmarkCaveImplementations(100, 5, 1025, 1025, 3);
    //CaveGenerator_base* base = new CaveGenerator_base(5, 10, 2, 50);
    //base->SetB({3});
    //base->SetS({2, 3});
    //CaveGenerator_mt_wMTcalc_alt_sum* altSum = new CaveGenerator_mt_wMTcalc_alt_sum(*base);

    //cout << base << "\n";
    //cout << altSum << "\n";


    //base->Tick(1);
    //altSum->TickMT(1);

    //cout << base << "\n";
    //cout << altSum << "\n";


    //delete base;
    //delete altSum;

    //BenchmarkCaveOTMT(100, 5, 2049, 2049, 3);

    //test(500, 5, 1025, 1025, 1);

    //BenchmarkCaveOTMT(10, 8, 2049, 2049, 2);
    //BenchmarkCaveOTMT(10, 8, 2049, 2049, 3);
    //BenchmarkCaveOTMT(10, 8, 2049, 2049, 4);
    //BenchmarkCaveOTMT(10, 8, 2049, 2049, 5);
    //BenchmarkCaveOTMT(10, 8, 2049, 2049, 6);

    //BenchmarkCaveThreadsSpeedUp(10, 4, 2049, 2049, 1, 4);

    /*CaveGenerator* cave = new CaveGenerator(15, 15, 2, true);
    cave->SetB({ 5, 6, 7, 8 });
    cave->SetS({ 4, 5, 6, 7, 8});
    cout << cave << std::endl;
    cave->Tick();
    cout << cave << std::endl;
    
    delete cave;*/
}

/*
=== Benchmark Configuration ===
benchmarkIterations: 100
operationsPerIteration: 5
width: 1025, height: 1025
threadCount: 2
===============================

Benchmarking Base (Single-threaded)...
Benchmarking Base Alt (Single-threaded)...
Benchmarking Base (Multi-threaded)...
Benchmarking Base Alt (Multi-threaded)...
Benchmarking mt_wMTcalc_alt_sum (Multi-threaded)...
Benchmarking mt_wMTcalc (Multi-threaded)...
Benchmarking mt_woMTcalc (Multi-threaded)...

=== Benchmark Results ===
Implementation: CaveGenerator_base (Tick)
  Type: Single-threaded
  Total Time: 1.31803 seconds
  Average Time per operation: 2.63606 ms

Implementation: CaveGenerator_base Alt (Tick)
  Type: Single-threaded
  Total Time: 0.777529 seconds
  Average Time per operation: 1.55506 ms

Implementation: CaveGenerator_base (TickMT)
  Type: Multi-threaded
  Total Time: 0.720156 seconds
  Average Time per operation: 1.44031 ms

Implementation: CaveGenerator_base Alt (TickMT)
  Type: Multi-threaded
  Total Time: 0.435069 seconds
  Average Time per operation: 0.870138 ms

Implementation: CaveGenerator_mt_wMTcalc_alt_sum (TickMT)
  Type: Multi-threaded
  Total Time: 0.427696 seconds
  Average Time per operation: 0.855393 ms

Implementation: CaveGenerator_mt_wMTcalc (TickMT)
  Type: Multi-threaded
  Total Time: 0.694365 seconds
  Average Time per operation: 1.38873 ms

Implementation: CaveGenerator_mt_woMTcalc (TickMT)
  Type: Multi-threaded
  Total Time: 0.699046 seconds
  Average Time per operation: 1.39809 ms

=== Performance vs Base Single-threaded ===
Speedup (CaveGenerator_base (Tick) / CaveGenerator_base Alt (Tick)): 1.69515x
Speedup (CaveGenerator_base (Tick) / CaveGenerator_base (TickMT)): 1.8302x
Speedup (CaveGenerator_base (Tick) / CaveGenerator_base Alt (TickMT)): 3.02947x
Speedup (CaveGenerator_base (Tick) / CaveGenerator_mt_wMTcalc_alt_sum (TickMT)): 3.08169x
Speedup (CaveGenerator_base (Tick) / CaveGenerator_mt_wMTcalc (TickMT)): 1.89818x
Speedup (CaveGenerator_base (Tick) / CaveGenerator_mt_woMTcalc (TickMT)): 1.88547x
*/