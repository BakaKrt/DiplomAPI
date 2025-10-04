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
    DLL_EXPORT HeightMap* CreateHeightMap(size_t width, size_t height, bool setRandomValue = true) {
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
    /// <returns>Указатель на карту высот</returns>
    HeightMap* HeightMap_Create(size_t width, size_t height, bool setRandomValue)
    {
        try {
            return new HeightMap(width, height, setRandomValue);
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
            obj->TickAsync(count);
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
}