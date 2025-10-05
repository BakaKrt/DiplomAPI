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

//#include "HeightMap.h"
#include "ExternDLL.h"

using std::vector;
using std::array;
using std::thread;

using byte = uint8_t;


//static void BenchmarkTickVsTickAsync(size_t benchmarkIterations, size_t operationsPerIteration, size_t width, size_t height, int threadCount = 2) {
//    using namespace std::chrono;
//    using std::cout;
//    if (operationsPerIteration == 0) {
//        cout << "Error: operationsPerIteration must be greater than 0.\n";
//        return;
//    }
//
//    cout << "benchmarkIterations: " << benchmarkIterations << " operationsPerIteration: "
//        << operationsPerIteration << "\nwidth: " << width << " height: " << height << " threadCount: " << threadCount << '\n';
//
//    duration<double> totalSyncTime = duration<double>(0.0);
//    duration<double> totalAsyncTime = duration<double>(0.0);
//
//    steady_clock::time_point start;
//    steady_clock::time_point end;
//
//    for (size_t x = 0; x < benchmarkIterations; x++) {
//        HeightMap sync_map = HeightMap(width, height, threadCount, true);
//        HeightMap async_map = sync_map; // Копируем из sync_map
//
//        // --- Измеряем асинхронную версию ---
//        start = steady_clock::now();
//        async_map.TickAsync(operationsPerIteration);
//        end = steady_clock::now();
//        totalAsyncTime += end - start;
//        //cout << "totalAsyncTime" << totalAsyncTime.count() << "\n";
//
//        // --- Измеряем синхронную версию ---
//        start = steady_clock::now();
//        sync_map.Tick(operationsPerIteration);
//        end = steady_clock::now();
//        totalSyncTime += end - start;
//        //cout << "totalSyncTime" << totalSyncTime.count() << "\n";
//    }
//
//    // Вычисляем среднее время на одну операцию (Tick или TickAsync)
//    double avgSyncTimeMs = (totalSyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;
//    double avgAsyncTimeMs = (totalAsyncTime.count() / (benchmarkIterations * operationsPerIteration)) * 1000.0;
//
//    // Выводим результаты
//    cout << "Benchmark Results:\n";
//    cout << "Total Sync Time: " << totalSyncTime.count() << " seconds\n";
//    cout << "Total Async Time: " << totalAsyncTime.count() << " seconds\n";
//    cout << "Average Sync Time per operation: " << avgSyncTimeMs << " ms\n";
//    cout << "Average Async Time per operation: " << avgAsyncTimeMs << " ms\n";
//
//    // Выводим коэффициент ускорения, если асинхронный метод быстрее (и делаем проверку на 0)
//    if (avgSyncTimeMs > 0.0) {
//        if (avgAsyncTimeMs > 0.0) {
//            double speedup = avgSyncTimeMs / avgAsyncTimeMs;
//            cout << "Speedup (Sync/Async): " << speedup << "x\n";
//        }
//        else {
//            cout << "Async time is 0, cannot calculate speedup.\n";
//        }
//    }
//    else {
//        cout << "Sync time is 0, cannot calculate speedup.\n";
//    }
//}


//int main()
//{
//    //using std::cout;
//
//    //BenchmarkTickVsTickAsync(50, 7, 1000, 1000, 2);
//    
//    /*Flat2DByte matrix = Flat2DByte(100000, 100000);
//    benchmark(1000000, 4096, size_t(1024 * 1024));*/
//
//    //Flat2DByte m = Flat2DByte(10, 20);
//    //cout << m;
//
//    
//
//    //HeightMap m = HeightMap(5, 10, 1);
//    //cout << m << "\n";
//    //int c = m.SetThreadCount(0);
//    //m.SetThreadCount(c / 4);
//    //m.TickAsync();
//    //cout << m << "\n";
//    //m.TickAsync();
//    //cout << m << "\n";
//    //m.TickAsync();
//    //cout << m << "\n";
//
//    //HeightMap map = HeightMap(5, 5);
//    //map.SetRules(
//    //    1, 1, 1,
//    //    1,    1,
//    //    1, 1, 1
//    //);
//    //std::cout << *map.GetMatrix() << "__________________\n";
//    //map.Tick();
//    //std::cout << *map.GetMatrix() << "__________________\n";
//    //map.Tick();
//    //std::cout << *map.GetMatrix() << "__________________\n";
//    //map.Tick();
//}