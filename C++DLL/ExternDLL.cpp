#include "ExternDLL.h"

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
    using namespace std::chrono;
    using std::cout;
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
    using namespace std::chrono;
    using std::cout;
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
        CaveGenerator* cave = new CaveGenerator(width, height, threadCount, true);
        CaveGenerator* async_cave = new CaveGenerator(*cave); // Копируем из sync_map

        // --- Измеряем асинхронную версию ---
        start = steady_clock::now();
        async_cave->TickMT(operationsPerIteration);
        end = steady_clock::now();
        totalAsyncTime += end - start;
        //cout << "totalAsyncTime" << totalAsyncTime.count() << "\n";

        // --- Измеряем синхронную версию ---
        start = steady_clock::now();
        cave->Tick(operationsPerIteration);
        end = steady_clock::now();
        totalSyncTime += end - start;
        //cout << "totalSyncTime" << totalSyncTime.count() << "\n";
        delete cave;
        delete async_cave;
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
    using namespace std::chrono;
    using std::cout;
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


int main()
{
    using std::cout;

    BenchmarkCaveOTMT(10, 8, 2049, 2049, 2);
    BenchmarkCaveOTMT(10, 8, 2049, 2049, 3);
    BenchmarkCaveOTMT(10, 8, 2049, 2049, 4);
    BenchmarkCaveOTMT(10, 8, 2049, 2049, 5);
    BenchmarkCaveOTMT(10, 8, 2049, 2049, 6);

    //BenchmarkCaveThreadsSpeedUp(10, 4, 2049, 2049, 1, 4);

    /*CaveGenerator* cave = new CaveGenerator(15, 15, 2, true);
    cave->SetB({ 5, 6, 7, 8 });
    cave->SetS({ 4, 5, 6, 7, 8});
    cout << cave << std::endl;
    cave->Tick();
    cout << cave << std::endl;
    
    delete cave;*/
}