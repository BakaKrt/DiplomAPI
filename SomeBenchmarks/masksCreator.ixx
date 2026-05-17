module;

export module MasksCreator;

import std;

using std::uint8_t;
using std::array;
using std::initializer_list;

export class MasksCreator
{
public:

    /// <summary>
    /// Возвращает таблицу поиска. Первые 16 байт - правило B, последние 16 - S
    /// </summary>
    /// <param name="B"></param>
    /// <param name="S"></param>
    /// <returns></returns>
    static array<uint8_t, 32> Generate32bitLUTByBS(initializer_list<uint8_t> B, initializer_list<uint8_t> S) {
        alignas(16) array<uint8_t, 32> res {0};

        for (auto n : B) if (n < 16) res[n] = 1;
        for (auto n : S) if (n < 16) res[n + 16] = 1;
        return res;
    }
};