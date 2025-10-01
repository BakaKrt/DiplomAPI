#include "ExternDLL.h"

extern "C" {

    // ������� ������ HeightMap � ������� ��������� �� ����

    DLL_EXPORT HeightMap* CreateHeightMap(size_t width, size_t height, bool setRandomValue = true) {
        try {
            return new HeightMap(width, height, setRandomValue);
        }
        catch (...) {
            return nullptr;
        }
    }

    HeightMap* HeightMap_Create(size_t width, size_t height, bool setRandomValue)
    {
        try {
            return new HeightMap(width, height, setRandomValue);
        }
        catch (...) {
            return nullptr;
        }
    }

    // ���������� �������
    DLL_EXPORT void HeightMap_SetRules(HeightMap* obj, const bool rules[8]) {
        if (obj && rules) {
            obj->SetRules(rules);
        }
    }



    // ���������� ������� �� ��������� ����������

    DLL_EXPORT void HeightMap_SetRulesEx(HeightMap* obj,
        bool r1, bool r2, bool r3, bool r4,
        bool r5, bool r6, bool r7, bool r8)
    {

        if (obj) {
            obj->SetRules(r1, r2, r3, r4, r5, r6, r7, r8);
        }
    }



    /// <summary>
    /// ��������� ���� ��� ��������� � ����� ������ (����� ������� ��� ��������� �������)
    /// </summary>
    /// <param name="obj">��������� �� ����� �����</param>
    DLL_EXPORT void HeightMap_Tick(HeightMap* obj) {
        if (obj) {
            obj->Tick();
        }
    }



    /// <summary>
    /// ����� Tick � ��������� �������
    /// </summary>
    /// <param name="obj">��������� �� ����� �����</param>
    DLL_EXPORT void HeightMap_TickMT(HeightMap* obj, int count) {
        if (obj) {
            obj->TickAsync(count);
        }
    }



    /// <summary>

    /// ���������� ���������� �������

    /// </summary>

    /// <param name="obj">��������� �� ����� �����</param>

    /// <param name="threadCount">���������� �������</param>

    /// <returns>������������� ���������� �������</returns>

    DLL_EXPORT int HeightMap_SetThreadCount(HeightMap* obj, const int threadCount) {

        if (obj) {

            return obj->SetThreadCount(threadCount);

        }

        return -1;

    }



    /// <summary>

    /// ������ ���������� �������

    /// </summary>

    /// <param name="obj">��������� �� ����� �����</param>

    /// <returns>���������� �������, ������������� � ����� �����</returns>

    DLL_EXPORT int HeightMap_GetThreadCount(HeightMap* obj) {

        if (obj) {

            return obj->GetThreadsCount();

        }

        return -1;

    }



    /// <summary>

    /// �������� ��������� �� �������

    /// </summary>

    /// <param name="obj">��������� �� ����� �����</param>

    /// <returns>��������� �� ������� (������� ������)</returns>

    DLL_EXPORT Flat2DByte* HeightMap_GetMatrix(HeightMap* obj) {

        if (obj) {

            return obj->GetMatrix();

        }

        return nullptr;

    }



    /// <summary>

    /// �������� ������ �����

    /// </summary>

    /// <param name="obj">��������� �� ����� �����</param>

    /// <returns>������ �����</returns>

    DLL_EXPORT size_t HeightMap_GetWidth(HeightMap* obj) {

        if (obj) {

            return obj->GetWidth();

        }

        return 0;

    }



    /// <summary>

    /// �������� ������ �����

    /// </summary>

    /// <param name="obj">��������� �� ����� �����</param>

    /// <returns>������ �����</returns>

    DLL_EXPORT size_t HeightMap_GetHeight(HeightMap* obj) {

        if (obj) {

            return obj->GetHeight();

        }

        return 0;

    }



    /// <summary>

    /// ���������� ������ HeightMap

    /// </summary>

    /// <param name="obj"></param>

    DLL_EXPORT void DestroyHeightMap(HeightMap* obj) {

        delete obj;

    }



    /// <summary>

    /// ���������� ������ HeightMap

    /// </summary>

    /// <param name="obj"></param>

    DLL_EXPORT void HeightMap_Destroy(HeightMap* obj) {

        delete obj;

    }



    // ==================== Flat2DByte API (��� ����������� �����) ====================



    // ������� ������ Flat2DByte � ������� ��������� �� ����

    DLL_EXPORT Flat2DByte* CreateFlat2DByte(size_t width, size_t height) {

        try {

            return new Flat2DByte(width, height);

        }

        catch (...) {

            return nullptr;

        }

    }



    // ���������� ��������

    DLL_EXPORT void Flat2DByte_SetData(Flat2DByte* obj, size_t x, size_t y, byte value) {

        if (obj) {

            try {

                obj->at(x, y) = value;

            }

            catch (const std::out_of_range&) {

                // ���� ����������

            }

        }

    }



    // �������� ��������

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



    // �������� ��������� �� ������

    DLL_EXPORT byte* Flat2DByte_GetDataPtr(Flat2DByte* obj) {

        if (obj) {

            return obj->data();

        }

        return nullptr;

    }



    // �������� ������ ������

    DLL_EXPORT size_t Flat2DByte_GetDataSize(Flat2DByte* obj) {

        if (obj) {

            return obj->capacity();

        }

        return 0;

    }



    // �������� ������

    DLL_EXPORT size_t Flat2DByte_GetWidth(Flat2DByte* obj) {

        if (obj) {

            return obj->width();

        }

        return 0;

    }



    // �������� ������

    DLL_EXPORT size_t Flat2DByte_GetHeight(Flat2DByte* obj) {

        if (obj) {

            return obj->height();

        }

        return 0;

    }



    // ���������� ������ Flat2DByte

    DLL_EXPORT void DestroyFlat2DByte(Flat2DByte* obj) {

        delete obj;

    }

}