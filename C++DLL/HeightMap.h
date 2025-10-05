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

    // ����������� �����������
    HeightMap(const HeightMap& other);

    // �������� ������������
    HeightMap& operator=(const HeightMap& other);

    /// <summary>
    /// ���������� ������� �� �������
    /// </summary>
    void SetMatrix(Flat2DByte* matrix);

    /// <summary>
    /// ���������� ������� ��� Tick() � TickAsync()
    /// </summary>
    /// <param name="rules">������ ������� ��������</param>
    void SetRules(const array<bool, 8> rules);

    /// <summary>
    /// ���������� ������� ��� Tick() � TickAsync()
    /// </summary>
    /// <param name="rules">������ ������� �������� [8]</param>
    void SetRules(const bool rules[8]);

    using cbool = const bool;

    /// <summary>
    /// ���������� ������� ��� Tick() � TickAsync()
    /// </summary>
    /// <param name="r1">������� ����� ����</param>
    /// <param name="r2">������� ����</param>
    /// <param name="r3">������� ������ ����</param>
    /// <param name="r4">����� ����</param>
    /// <param name="r5">������ ����</param>
    /// <param name="r6">������ ����� ����</param>
    /// <param name="r7">������ ����</param>
    /// <param name="r8">������ ������ ����</param>
    void SetRules(cbool r1, cbool r2, cbool r3, cbool r4, cbool r5, cbool r6, cbool r7, cbool r8);

    /// <summary>
    /// ������������ Tick
    /// </summary>
    void Tick(const size_t count = 1) noexcept;

    /// <summary>
    /// Tick � ThreadsCount �������
    /// </summary>
    void TickMT(const size_t count = 1) noexcept;

    /// <summary>
    /// �������� ���������� ������������� �������
    /// </summary>
    /// <returns>���������� ������� ��� TickAsync</returns>
    size_t GetThreadsCount() const;

    /// <summary>
    /// �������� ������ ����� �����
    /// </summary>
    /// <returns>������</returns>
    size_t GetWidth();

    /// <summary>
    /// �������� ������ ����� �����
    /// </summary>
    /// <returns>������</returns>
    size_t GetHeight();

    /// <summary>
    /// �������� ��������� �� ������� (������� ������)
    /// </summary>
    /// <returns>��������� �� �������</returns>
    Flat2DByte* GetMatrix();

    std::ostream& operator <<(std::ostream& stream);
    friend std::ostream& operator <<(std::ostream& stream, HeightMap& map);

    ~HeightMap();
};