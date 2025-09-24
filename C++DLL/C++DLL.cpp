#include <iostream>
#include <cstddef> // For std::byte
#include <cstdint>
#include <vector>
#include <array>
#include <random>
#include <chrono>


#ifdef _DEBUG
    #include <stdexcept> // Для std::out_of_range
    
#endif


#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif


using std::vector;
using std::array;

using byte = uint8_t;

class Flat2DByte {
private:
    size_t Capacity;
    size_t Width, Height;
    byte* Array = nullptr;
    friend class HeightMap;
public:
    Flat2DByte() {
        Capacity = 0;
        Width = 0, Height = 0;
    }

    Flat2DByte(size_t Width, size_t Height) noexcept
    {
        this->Width  = Width;
        this->Height = Height;
        this->Capacity = this->Width * this->Height;
        this->Array = new byte[Capacity]();
    }

    Flat2DByte(const Flat2DByte& other) {
        Width = other.Width;
        Height = other.Height;
        Capacity = other.Capacity;

        if (other.Array) {
            Array = new byte[Capacity];
            for (size_t i = 0; i < Capacity; ++i) {
                Array[i] = other.Array[i];
            }
        }
        else {
            Array = nullptr;
        }
    }

    Flat2DByte& operator=(const Flat2DByte& other) {
        if (this != &other) { // Проверка на самоприсваивание
            delete[] Array; // Освобождаем старую память

            Width = other.Width;
            Height = other.Height;
            Capacity = other.Capacity;

            if (other.Array) {
                Array = new byte[Capacity];
                for (size_t i = 0; i < Capacity; ++i) {
                    Array[i] = other.Array[i];
                }
            }
            else {
                Array = nullptr;
            }
        }
        return *this;
    }

    inline byte& at(size_t x) noexcept {
        #ifdef _DEBUG
        if (x >= Capacity) {
            throw std::out_of_range("Index out of range");
        }
        #endif // DEBUG
        return Array[x];
    }

    inline byte& at(size_t x, size_t y) noexcept {
        #ifdef _DEBUG
        if (x >= Width || y >= Height) {
            throw std::out_of_range("Index out of range");
        }
        #endif // DEBUG
        return Array[y * Width + x];
    }

    inline byte& operator[] (size_t posX) noexcept {
        return Array[posX];
    }

    inline byte* data() noexcept {
        return Array;
    }
    inline size_t capacity() const noexcept { return Capacity; }
    inline size_t width()    const noexcept { return Width; }
    inline size_t height()   const noexcept { return Height; }

    friend std::ostream& operator <<(std::ostream& stream, Flat2DByte& data) noexcept {
        for (size_t i = 0; i < data.Capacity;)
        {
            stream << (int)data.at(i) << "\t";
            i++;
            if (i % data.Width == 0 && i != 0) stream << '\n';
            else stream << ' ';
        }
        return stream;
    }

    ~Flat2DByte() noexcept {
        delete[] Array;
    }
};

class HeightMap {
private:
    Flat2DByte* _MainMatrix;
    Flat2DByte* _SecondMatrix;

    array<bool, 8> _rules = { 1, 1, 1, 1, 1, 1, 1, 1 };
    array<bool, 8> _baseRules = _rules;

    byte _rulesLen = 8;

    size_t Width, Height;

    byte GetAVGSum(size_t x, size_t y) {
#ifdef _DEBUG
        if (x >= Width || y >= Height) {
            throw std::out_of_range("");
        }
#endif

        size_t res = 0;

        // Определяем направления соседей в порядке, соответствующем _rules
        // [0] [1] [2]    (-1,-1) (0,-1) (1,-1)
        // [3]  X  [4]    (-1, 0)        (1, 0)
        // [5] [6] [7]    (-1, 1) (0, 1) (1, 1)

        int dx[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };
        int dy[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };

        byte count = 0;
        for (int i = 0; i < 8; i++) {
            // Проверяем, нужно ли учитывать этого соседа согласно правилам
            if (!_rules[i]) {
                continue;
            }

            int nx = (int)x + dx[i];
            int ny = (int)y + dy[i];

            // Проверяем границы массива
            if (nx >= 0 && nx < (int)Width && ny >= 0 && ny < (int)Height) {
                byte value = _MainMatrix->at(nx, ny);
                if (value != 0) {
                    res += value;
                    count++;
                }
            }
        }

        return count > 0 ? (byte)(res / count) : 0;
    }

    void InitMatrixRandomValue() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dis(0, 255);

        size_t Count = this->Width * this->Height;

        for (size_t x = 0; x < Count; x++) {
            _MainMatrix->at(x) = (byte)dis(gen);
        }
    }

public:

    HeightMap(size_t Width, size_t Height, bool SetRandomValue = true)
    {
        this->Width = Width;
        this->Height = Height;
        this->_MainMatrix = new Flat2DByte(Width, Height);
        this->_SecondMatrix = new Flat2DByte(Width, Height);

        if (SetRandomValue) {
            InitMatrixRandomValue();
        }
    }

    // Конструктор копирования
    HeightMap(const HeightMap& other) {
        this->Width = other.Width;
        this->Height = other.Height;
        this->_rules = other._rules;
        this->_baseRules = other._baseRules;
        this->_rulesLen = other._rulesLen;

        this->_MainMatrix = new Flat2DByte(*other._MainMatrix);
        this->_SecondMatrix = new Flat2DByte(*other._SecondMatrix);
    }

    // Оператор присваивания
    HeightMap& operator=(const HeightMap& other) {
        if (this != &other) {
            delete _MainMatrix;
            delete _SecondMatrix;

            this->Width = other.Width;
            this->Height = other.Height;
            this->_rules = other._rules;
            this->_baseRules = other._baseRules;
            this->_rulesLen = other._rulesLen;

            this->_MainMatrix = new Flat2DByte(*other._MainMatrix);
            this->_SecondMatrix = new Flat2DByte(*other._SecondMatrix);
        }
        return *this;
    }

    void SetRules(const array<bool, 8> rules) {
        this->_rules = rules;
        this->_rulesLen = 0;
        for (const auto& val : _rules) {
            _rulesLen += val;
        }
    }

    void SetRules(const bool rules[8]) {
        this->_rules = { rules[0], rules[1], rules[2], rules[3], rules[4], rules[5], rules[6], rules[7] };
        this->_rulesLen = 0;
        for (const auto& val : _rules) {
            _rulesLen += val;
        }
    }

    using cbool = const bool;

    void SetRules(cbool r1, cbool r2, cbool r3, cbool r4, cbool r5, cbool r6, cbool r7, cbool r8) {
        this->_rules = { r1, r2, r3, r4, r5, r6, r7, r8 };
        this->_rulesLen = 0;
        for (const auto& val : _rules) {
            _rulesLen += val;
        }
    }

    void Tick() {
        for (size_t x = 0; x < this->Width; x++) {
            for (size_t y = 0; y < this->Height; y++) { // Исправлено: было x < this->Height
                byte AVG = GetAVGSum(x, y);
                _SecondMatrix->at(x, y) = AVG;
            }
        }
        // Обмениваем указатели вместо копирования данных для лучшей производительности
        Flat2DByte* temp = _MainMatrix;
        _MainMatrix = _SecondMatrix;
        _SecondMatrix = temp;
    }

    size_t GetWidth() {
        return _MainMatrix->Width;
    }

    size_t GetHeight() {
        return _MainMatrix->Height;
    }

    Flat2DByte* GetMatrix() {
        return _MainMatrix;
    }

    ~HeightMap() {
        delete _MainMatrix;
        delete _SecondMatrix;
    }
};

// DLL API реализация
extern "C" {
    // Создать объект HeightMap и вернуть указатель на него
    DLL_EXPORT HeightMap* CreateHeightMap(size_t width, size_t height, bool setRandomValue) {
        try {
            return new HeightMap(width, height, setRandomValue);
        }
        catch (...) {
            return nullptr;
        }
    }

    // Установить правила
    DLL_EXPORT void HeightMap_SetRules(HeightMap* obj, const bool rules[8]) {
        if (obj && rules) {
            obj->SetRules(rules);
        }
    }

    // Установить правила по отдельным параметрам
    DLL_EXPORT void HeightMap_SetRulesEx(HeightMap* obj,
        bool r1, bool r2, bool r3, bool r4,
        bool r5, bool r6, bool r7, bool r8) {
        if (obj) {
            obj->SetRules(r1, r2, r3, r4, r5, r6, r7, r8);
        }
    }

    // Выполнить один шаг симуляции
    DLL_EXPORT void HeightMap_Tick(HeightMap* obj) {
        if (obj) {
            obj->Tick();
        }
    }

    // Получить указатель на матрицу
    DLL_EXPORT Flat2DByte* HeightMap_GetMatrix(HeightMap* obj) {
        if (obj) {
            return obj->GetMatrix();
        }
        return nullptr;
    }

    // Получить ширину карты
    DLL_EXPORT size_t HeightMap_GetWidth(HeightMap* obj) {
        if (obj) {
            return obj->GetWidth();
        }
        return 0;
    }

    // Получить высоту карты
    DLL_EXPORT size_t HeightMap_GetHeight(HeightMap* obj) {
        if (obj) {
            return obj->GetHeight();
        }
        return 0;
    }

    // Уничтожить объект HeightMap
    DLL_EXPORT void DestroyHeightMap(HeightMap* obj) {
        delete obj;
    }

    // ==================== Flat2DByte API (уже реализовано ранее) ====================

    // Создать объект Flat2DByte и вернуть указатель на него
    DLL_EXPORT Flat2DByte* CreateFlat2DByte(size_t width, size_t height) {
        try {
            return new Flat2DByte(width, height);
        }
        catch (...) {
            return nullptr;
        }
    }

    // Установить значение
    DLL_EXPORT void Flat2DByte_SetData(Flat2DByte* obj, size_t x, size_t y, byte value) {
        if (obj) {
            try {
                obj->at(x, y) = value;
            }
            catch (const std::out_of_range&) {
                // Тихо игнорируем
            }
        }
    }

    // Получить значение
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

    // Получить указатель на данные
    DLL_EXPORT byte* Flat2DByte_GetDataPtr(Flat2DByte* obj) {
        if (obj) {
            return obj->data();
        }
        return nullptr;
    }

    // Получить размер данных
    DLL_EXPORT size_t Flat2DByte_GetDataSize(Flat2DByte* obj) {
        if (obj) {
            return obj->capacity();
        }
        return 0;
    }

    // Получить ширину
    DLL_EXPORT size_t Flat2DByte_GetWidth(Flat2DByte* obj) {
        if (obj) {
            return obj->width();
        }
        return 0;
    }

    // Получить высоту
    DLL_EXPORT size_t Flat2DByte_GetHeight(Flat2DByte* obj) {
        if (obj) {
            return obj->height();
        }
        return 0;
    }

    // Уничтожить объект Flat2DByte
    DLL_EXPORT void DestroyFlat2DByte(Flat2DByte* obj) {
        delete obj;
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
    //Flat2DByte matrix = Flat2DByte(100000, 100000);
    //benchmark(1000000, 4096, size_t(1024 * 1024));

    Flat2DByte m = Flat2DByte(10, 20);
    cout << m;

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