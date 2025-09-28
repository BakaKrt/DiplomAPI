#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

// ������� ��������� .cpp ������, �������� ������ ���������
#include "Flat2DByte.h"
#include "HeightMap.h"

extern "C" {
    /// <summary>
    /// ������� ������� �����
    /// </summary>
    /// <param name="width">������ (size_t)</param>
    /// <param name="height">������ (size_t)</param>
    /// <param name="setRandomValue">��������� ������� ����� ���������� ����������? (true �� ���������)</param>
    /// <returns>��������� �� ������� �����</returns>
    DLL_EXPORT HeightMap* CreateHeightMap(size_t width, size_t height, bool setRandomValue);

    /// <summary>
    /// ������� ������� �����
    /// </summary>
    /// <param name="width">������ (size_t)</param>
    /// <param name="height">������ (size_t)</param>
    /// <param name="setRandomValue">��������� ������� ����� ���������� ����������? (true �� ���������)</param>
    /// <returns>��������� �� ������� �����</returns>
    DLL_EXPORT HeightMap* HeightMap_Create(size_t width, size_t height, bool setRandomValue);

    /// <summary>
    /// ���������� �������
    /// </summary>
    /// <param name="obj">��������� �� ����� �����</param>
    /// <param name="rules">������ ������� �������� ������ 8</param>
    DLL_EXPORT void HeightMap_SetRules(HeightMap* obj, const bool rules[8]);
    
    /// <summary>
    /// ���������� ������� ��� Tick() � TickAsync()
    /// </summary>
    /// <param name="obj">��������� �� ����� �����</param>
    /// <param name="r1">������� ����� ����</param>
    /// <param name="r2">������� ����</param>
    /// <param name="r3">������� ������ ����</param>
    /// <param name="r4">����� ����</param>
    /// <param name="r5">������ ����</param>
    /// <param name="r6">������ ����� ����</param>
    /// <param name="r7">������ ����</param>
    /// <param name="r8">������ ������ ����</param>
    DLL_EXPORT void HeightMap_SetRulesEx(HeightMap* obj,
        bool r1, bool r2, bool r3, bool r4,
        bool r5, bool r6, bool r7, bool r8);
    
    /// <summary>
    /// ������������ ���������� Tick()
    /// </summary>
    /// <param name="obj">��������� �� ����� �����</param>
    DLL_EXPORT void HeightMap_Tick(HeightMap* obj);
    
    /// <summary>
    /// ������������� ���������� Tick()
    /// </summary>
    /// <param name="obj">��������� �� ����� �����</param>
    DLL_EXPORT void HeightMap_TickMT(HeightMap* obj);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <param name="threadCount"></param>
    /// <returns></returns>
    DLL_EXPORT int HeightMap_SetThreadCount(HeightMap* obj, const int threadCount);
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    DLL_EXPORT int HeightMap_GetThreadCount(HeightMap* obj);
    
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
}