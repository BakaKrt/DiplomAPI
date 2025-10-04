#include <iostream>
#include <cstddef> // For std::byte
#include <cstdint>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include <thread>



#ifdef _DEBUG
    #include <stdexcept> // Для std::out_of_range
    
#endif

#include "HeightMap.h"
//#include "ExternDLL.h"

using std::vector;
using std::array;
using std::thread;

using byte = uint8_t;



// DLL API реализация



static void BenchmarkTickVsTickAsync(size_t benchmarkIterations, size_t operationsPerIteration, size_t width, size_t height, int threadCount = 2) {
    if (operationsPerIteration == 0) {
        std::cout << "Error: operationsPerIteration must be greater than 0.\n";
        return;
    }

    std::chrono::duration<double> totalSyncTime = std::chrono::duration<double>(0.0);
    std::chrono::duration<double> totalAsyncTime = std::chrono::duration<double>(0.0);

    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    for (size_t x = 0; x < benchmarkIterations; x++) {
        // Создаем объекты до измерения времени
        HeightMap sync_map = HeightMap(width, height, threadCount, true);
        HeightMap async_map = sync_map; // Копируем из sync_map

        // --- Измеряем синхронную версию ---
        start = std::chrono::steady_clock::now(); // Используем steady_clock
        for (size_t i = 0; i < operationsPerIteration; i++) {
            sync_map.Tick();
        }
        end = std::chrono::steady_clock::now();
        totalSyncTime += end - start;

        // --- Измеряем асинхронную версию ---
        start = std::chrono::steady_clock::now(); // Используем steady_clock
        async_map.TickAsync(operationsPerIteration); // Предполагается, что TickAsync теперь корректно реализован

        end = std::chrono::steady_clock::now();
        totalAsyncTime += end - start;
    }

    // Вычисляем среднее время на одну операцию (Tick или TickAsync)
    double avgSyncTimeMs = (totalSyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;
    double avgAsyncTimeMs = (totalAsyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;

    // Выводим результаты
    std::cout << "Benchmark Results:\n";
    std::cout << "Total Sync Time: " << totalSyncTime.count() << " seconds\n";
    std::cout << "Total Async Time: " << totalAsyncTime.count() << " seconds\n";
    std::cout << "Average Sync Time per operation: " << avgSyncTimeMs << " ms\n";
    std::cout << "Average Async Time per operation: " << avgAsyncTimeMs << " ms\n";

    // Выводим коэффициент ускорения, если асинхронный метод быстрее (и делаем проверку на 0)
    if (avgSyncTimeMs > 0.0) {
        if (avgAsyncTimeMs > 0.0) {
            double speedup = avgSyncTimeMs / avgAsyncTimeMs;
            std::cout << "Speedup (Sync/Async): " << speedup << "x\n";
        }
        else {
            std::cout << "Async time is 0, cannot calculate speedup.\n";
        }
    }
    else {
        std::cout << "Sync time is 0, cannot calculate speedup.\n";
    }
}

////#ifdef _DEBUG
//void inline static benchmark(size_t count, size_t sizeX, size_t sizeY) {
//    using std::cout;
//    size_t capacity = sizeX * sizeY;
//
//    std::chrono::duration<double> duration1_write = std::chrono::duration<double>(0.0);
//    std::chrono::duration<double> duration2_write = std::chrono::duration<double>(0.0);
//    std::chrono::duration<double> duration3_write = std::chrono::duration<double>(0.0);
//
//    std::chrono::duration<double> duration1_read = std::chrono::duration<double>(0.0);
//    std::chrono::duration<double> duration2_read = std::chrono::duration<double>(0.0);
//    std::chrono::duration<double> duration3_read = std::chrono::duration<double>(0.0);
//
//    size_t posX = sizeX / 2;
//    size_t posY = sizeY / 2;
//
//    volatile byte _tempData = 0x0;
//    
//    auto start = std::chrono::high_resolution_clock::now();
//    auto end = std::chrono::high_resolution_clock::now();
//
//
//#pragma region time1
//    {
//        byte* _arr1 = new byte[sizeX * sizeY];
//        start = std::chrono::high_resolution_clock::now();
//        for (size_t x = 0; x < capacity; x++) {
//            _arr1[x] = 0;
//        }
//        end = std::chrono::high_resolution_clock::now();
//        duration1_write = end - start;
//
//        start = std::chrono::high_resolution_clock::now();
//        for (size_t i = 0; i < count; i++) {
//            _tempData = _arr1[posX];
//            _tempData = _arr1[posX - 1];
//            _tempData = _arr1[posX - 2];
//            _tempData = _arr1[posX - 3];
//        }
//        end = std::chrono::high_resolution_clock::now();
//        duration1_read = end - start;
//
//        delete[]_arr1;
//    }
//#pragma endregion
//
//#pragma region time2
//    {
//        Flat2DByte _arr2 = Flat2DByte(sizeX, sizeY);
//        start = std::chrono::high_resolution_clock::now();
//        for (size_t x = 0; x < capacity; x++) {
//            _arr2[x] = 0;
//        }
//        end = std::chrono::high_resolution_clock::now();
//        duration2_write += end - start;
//
//        start = std::chrono::high_resolution_clock::now();
//        for (size_t i = 0; i < count; i++) {
//            _tempData = _arr2[posX];
//            _tempData = _arr2[posX - 1];
//            _tempData = _arr2[posX - 2];
//            _tempData = _arr2[posX - 3];
//        }
//        end = std::chrono::high_resolution_clock::now();
//        duration2_read = end - start;
//    }
//#pragma endregion
//
//
//#pragma region time3
//    {
//        std::vector<std::vector<byte>> _arr3(sizeX, std::vector<byte>(sizeY));
//
//        start = std::chrono::high_resolution_clock::now();
//        for (size_t x = 0; x < sizeX; x++) {
//            for (size_t y = 0; y < sizeY; y++) {
//                _arr3[x][y] = 0;
//            }
//        }
//        end = std::chrono::high_resolution_clock::now();
//        duration3_write += end - start;
//
//        start = std::chrono::high_resolution_clock::now();
//        for (size_t i = 0; i < count; i++) {
//            _tempData = _arr3[posX][posY];
//            _tempData = _arr3[posX - 1][posY];
//            _tempData = _arr3[posX - 2][posY];
//            _tempData = _arr3[posX - 3][posY];
//        }
//        end = std::chrono::high_resolution_clock::now();
//        duration3_read = end - start;
//    }
//#pragma endregion
//
//    cout << "Write time byte*\t\t" << duration1_write.count() << "   \t" << "100%\n";
//    cout << "Write time Flat2DByte\t\t" << duration2_write.count() << "   \t" << duration2_write.count() / duration1_write.count() * 100 << "%\n";
//    cout << "Write time vector<vector<byte>>\t" << duration3_write.count() << "\t" << duration3_write.count() / duration1_write.count() * 100 << "%\n";
//
//
//    cout << "Read time byte*\t\t\t" << duration1_read.count() << "\t" << "100%\n";
//    cout << "Read time Flat2DByte\t\t" << duration2_read.count() << "\t" << duration2_read.count() / duration1_read.count() * 100 << "%\n";
//    cout << "Read time vector<vector<byte>>\t" << duration3_read.count() << "\t" << duration3_read.count() / duration1_read.count() * 100 << "%\n";
//}

//#endif

int main()
{
    using std::cout;

    BenchmarkTickVsTickAsync(1, 7, 1000000, 2*1024, 4);
    
    /*Flat2DByte matrix = Flat2DByte(100000, 100000);
    benchmark(1000000, 4096, size_t(1024 * 1024));*/

    //Flat2DByte m = Flat2DByte(10, 20);
    //cout << m;

    

    //HeightMap m = HeightMap(5, 10, 1);
    //cout << m << "\n";
    //int c = m.SetThreadCount(0);
    //m.SetThreadCount(c / 4);
    //m.TickAsync();
    //cout << m << "\n";
    //m.TickAsync();
    //cout << m << "\n";
    //m.TickAsync();
    //cout << m << "\n";

    //HeightMap map = HeightMap(5, 5);
    //map.SetRules(
    //    1, 1, 1,
    //    1,    1,
    //    1, 1, 1
    //);
    //std::cout << *map.GetMatrix() << "__________________\n";
    //map.Tick();
    //std::cout << *map.GetMatrix() << "__________________\n";
    //map.Tick();
    //std::cout << *map.GetMatrix() << "__________________\n";
    //map.Tick();
}