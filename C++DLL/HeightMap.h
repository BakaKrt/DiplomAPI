#pragma once

#include "Flat2DByte.h"

#include <array>

using std::array;

class HeightMap {
private:
    Flat2DByte* _MainMatrix;
    Flat2DByte* _SecondMatrix;

    array<bool, 8> _rules = { 1, 1, 1, 1, 1, 1, 1, 1 };

    byte _rulesLen = 8;

    size_t ThreadsCount = 1;

    size_t Width, Height;


    byte GetAVGSum(size_t x, size_t y) noexcept;

    void InitMatrixRandomValue() noexcept;

    void TickAsyncRealization(size_t LineFrom, size_t LineTo);
public:

    HeightMap(size_t Width, size_t Height, bool SetRandomValue = true);

    HeightMap(size_t Width, size_t Height, int ThreadCount = 1, bool SetRandomValue = true);

    // Конструктор копирования
    HeightMap(const HeightMap& other);

    // Оператор присваивания
    HeightMap& operator=(const HeightMap& other);

    /// <summary>
    /// Установить матрицу от другого
    /// </summary>
    void SetMatrix(Flat2DByte* matrix);

    /// <summary>
    /// Установить правила для Tick() и TickAsync()
    /// </summary>
    /// <param name="rules">массив булевых значений</param>
    void SetRules(const array<bool, 8> rules);

    /// <summary>
    /// Установить правила для Tick() и TickAsync()
    /// </summary>
    /// <param name="rules">массив булевых значений [8]</param>
    void SetRules(const bool rules[8]);

    using cbool = const bool;

    /// <summary>
    /// Установить правила для Tick() и TickAsync()
    /// </summary>
    /// <param name="r1">Верхний левый блок</param>
    /// <param name="r2">Верхний блок</param>
    /// <param name="r3">Верхний правый блок</param>
    /// <param name="r4">Левый блок</param>
    /// <param name="r5">Правый блок</param>
    /// <param name="r6">Нижний левый блок</param>
    /// <param name="r7">Нижний блок</param>
    /// <param name="r8">Нижний правый блок</param>
    void SetRules(cbool r1, cbool r2, cbool r3, cbool r4, cbool r5, cbool r6, cbool r7, cbool r8);

    /// <summary>
    /// Однопоточный Tick
    /// </summary>
    void Tick(const size_t count = 1) noexcept;

    /// <summary>
    /// Tick в ThreadsCount потоков
    /// </summary>
    void TickMT(const size_t count = 1) noexcept;

    /// <summary>
    /// Получить количество установленных потоков
    /// </summary>
    /// <returns>Количество потоков для TickAsync</returns>
    size_t GetThreadsCount() const;

    /// <summary>
    /// Получить ширину карты высот
    /// </summary>
    /// <returns>Ширина</returns>
    size_t GetWidth();

    /// <summary>
    /// Получить высоту карты высот
    /// </summary>
    /// <returns>Высота</returns>
    size_t GetHeight();

    /// <summary>
    /// Получить указатель на матрицу (плоский массив)
    /// </summary>
    /// <returns>Указатель на матрицу</returns>
    Flat2DByte* GetMatrix();

    std::ostream& operator <<(std::ostream& stream);
    friend std::ostream& operator <<(std::ostream& stream, HeightMap& map);

    ~HeightMap();
};