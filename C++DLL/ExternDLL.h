#ifdef _WIN32
#pragma message("Compiling for WIN32")
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

// Уберите включение .cpp файлов, оставьте только заголовки
#include "Flat2DByte.h"
#include "Flat2DFloat.h"
#include "HeightMap.h"
#include "CaveGenerator.h"

extern "C" {
    /// <summary>
    /// Создать матрицу высот
    /// </summary>
    /// <param name="width">Ширина (size_t)</param>
    /// <param name="height">Высота (size_t)</param>
    /// <param name="setRandomValue">Заполнить матрицу высот случайными значениями? (true по умолчанию)</param>
    /// <returns>Указатель на матрицу высот</returns>
    DLL_EXPORT HeightMap* CreateHeightMap(size_t width, size_t height, bool setRandomValue = true);

    /// <summary>
    /// Создать матрицу высот
    /// </summary>
    /// <param name="width">Ширина (size_t)</param>
    /// <param name="height">Высота (size_t)</param>
    /// <param name="setRandomValue">Заполнить матрицу высот случайными значениями? (true по умолчанию)</param>
    /// <returns>Указатель на матрицу высот</returns>
    //DLL_EXPORT HeightMap* HeightMap_Create(size_t width, size_t height, bool setRandomValue = true);
    
    /// <summary>
    /// Создать карту высот
    /// </summary>
    /// <param name="width">Ширина</param>
    /// <param name="height">Высота</param>
    /// <param name="threadCount">Количество потоков</param>
    /// <param name="setRandomValue">Заполнить случайными значениями</param>
    /// <returns>Указатель на карту высот</returns>
    DLL_EXPORT HeightMap* HeightMap_Create(size_t width, size_t height, int threadCount = 1, bool setRandomValue = true);

    /// <summary>
    /// Установить правила
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="rules">Массив булевых значений длиной 8</param>
    DLL_EXPORT void HeightMap_SetRules(HeightMap* obj, const bool rules[8]);
    
    /// <summary>
    /// Установить правила для Tick() и TickAsync()
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="r1">Верхний левый блок</param>
    /// <param name="r2">Верхний блок</param>
    /// <param name="r3">Верхний правый блок</param>
    /// <param name="r4">Левый блок</param>
    /// <param name="r5">Правый блок</param>
    /// <param name="r6">Нижний левый блок</param>
    /// <param name="r7">Нижний блок</param>
    /// <param name="r8">Нижний правый блок</param>
    DLL_EXPORT void HeightMap_SetRulesEx(HeightMap* obj,
        bool r1, bool r2, bool r3, bool r4,
        bool r5, bool r6, bool r7, bool r8);
    
    /// <summary>
    /// Однопоточная реализация Tick()
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    DLL_EXPORT void HeightMap_Tick(HeightMap* obj, size_t count = 1);
    
    /// <summary>
    /// Многопоточная реализация Tick()
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    DLL_EXPORT void HeightMap_TickMT(HeightMap* obj, int count = 1);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t HeightMap_GetThreadCount(HeightMap* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT Flat2DByte* HeightMap_GetMatrix(HeightMap* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t HeightMap_GetWidth(HeightMap* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t HeightMap_GetHeight(HeightMap* obj);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT void HeightMap_Normalize(HeightMap* obj);

    /// <summary>
    /// Кнопка сделать хорошо
    /// </summary>
    /// <param name="obj">Указатель на карту высот</param>
    /// <param name="type">Тип: 0, 1, 2</param>
    DLL_EXPORT void HeightMap_MakeGood(HeightMap* obj, int type = 1);

    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    DLL_EXPORT void DestroyHeightMap(HeightMap* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    DLL_EXPORT void HeightMap_Destroy(HeightMap* obj);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <returns></returns>
    DLL_EXPORT Flat2DByte* CreateFlat2DByte(size_t width, size_t height);

    /// <summary>
    /// 
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    DLL_EXPORT Flat2DByte* Flat2DByte_CreateFromPtr(Flat2DByte*);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="value"></param>
    DLL_EXPORT void Flat2DByte_SetData(Flat2DByte* obj, size_t x, size_t y, byte value);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    DLL_EXPORT byte Flat2DByte_GetData(Flat2DByte* obj, size_t x, size_t y);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT byte* Flat2DByte_GetDataPtr(Flat2DByte* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t Flat2DByte_GetDataSize(Flat2DByte* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t Flat2DByte_GetWidth(Flat2DByte* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT size_t Flat2DByte_GetHeight(Flat2DByte* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    DLL_EXPORT void DestroyFlat2DByte(Flat2DByte* obj);



    DLL_EXPORT Flat2DFloat* CreateFlat2DFloat(size_t width, size_t height);


    DLL_EXPORT void Flat2DFloat_SetData(Flat2DFloat* obj, size_t x, size_t y, byte value);


    DLL_EXPORT float Flat2DFloat_GetData(Flat2DFloat* obj, size_t x, size_t y);


    DLL_EXPORT float* Flat2DFloat_GetDataPtr(Flat2DFloat* obj);


    DLL_EXPORT size_t Flat2DFloat_GetDataSize(Flat2DFloat* obj);


    DLL_EXPORT size_t Flat2DFloat_GetWidth(Flat2DFloat* obj);


    DLL_EXPORT size_t Flat2DFloat_GetHeight(Flat2DFloat* obj);


    DLL_EXPORT void DestroyFlat2DFloat(Flat2DFloat* obj);
}