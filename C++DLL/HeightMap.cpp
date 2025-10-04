#include "HeightMap.h"

byte HeightMap::GetAVGSum(size_t x, size_t y) noexcept {
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

    constexpr int dx[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };
    constexpr int dy[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };

    byte count = 0;
    for (int i = 0; i < 8; i++) {
        // Проверяем, нужно ли учитывать этого соседа согласно правилам
        if (!_rules[i]) {
            continue;
        }

        size_t nx = x + dx[i];
        size_t ny = y + dy[i];

        // Проверяем границы массива
        if (nx >= 0 && nx < Width && ny >= 0 && ny < Height) {
            byte value = _MainMatrix->at(nx, ny);
            res += value;
            count++;
        }
    }

    return count > 0 ? (byte)(res / count) : 0;
}

void HeightMap::InitMatrixRandomValue() noexcept {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(0, 255);

    size_t Count = this->Width * this->Height;

    for (size_t x = 0; x < Count; x++) {
        _MainMatrix->at(x) = (byte)dis(gen);
    }
}

void HeightMap::TickAsyncRealization(const size_t LineFrom, const size_t LineTo) {
    size_t iterator = LineFrom;
    for (; iterator < LineTo; iterator++) {
        for (size_t y = 0; y < this->Width; y++) {
            byte AVG = GetAVGSum(y, iterator);
            _SecondMatrix->at(y, iterator) = AVG;
        }
    }
}

HeightMap::HeightMap(size_t Width, size_t Height, bool SetRandomValue)
{
    this->Width = Width;
    this->Height = Height;
    this->_MainMatrix = new Flat2DByte(Width, Height);
    this->_SecondMatrix = new Flat2DByte(Width, Height);

    if (SetRandomValue) {
        InitMatrixRandomValue();
    }
}

HeightMap::HeightMap(size_t Width, size_t Height, int ThreadCount, bool SetRandomValue)
{
    this->Width = Width;
    this->Height = Height;
    this->_MainMatrix = new Flat2DByte(Width, Height);
    this->_SecondMatrix = new Flat2DByte(Width, Height);
    this->ThreadsCount = ThreadCount;

    if (SetRandomValue) {
        InitMatrixRandomValue();
    }
}

// Конструктор копирования
HeightMap::HeightMap(const HeightMap& other) {
    this->Width = other.Width;
    this->Height = other.Height;
    this->_rules = other._rules;
    this->_rulesLen = other._rulesLen;

    this->_MainMatrix = new Flat2DByte(*other._MainMatrix);
    this->_SecondMatrix = new Flat2DByte(*other._SecondMatrix);
}

// Оператор присваивания
HeightMap& HeightMap::operator=(const HeightMap& other) {
    if (this != &other) {
        delete _MainMatrix;
        delete _SecondMatrix;

        this->Width = other.Width;
        this->Height = other.Height;
        this->_rules = other._rules;
        this->_rulesLen = other._rulesLen;

        this->_MainMatrix = new Flat2DByte(*other._MainMatrix);
        this->_SecondMatrix = new Flat2DByte(*other._SecondMatrix);
    }
    return *this;
}

void HeightMap::SetRules(const array<bool, 8> rules) {
    this->_rules = rules;
    this->_rulesLen = 0;
    for (const auto& val : _rules) {
        _rulesLen += val;
    }
}

void HeightMap::SetRules(const bool rules[8]) {
    this->_rules = { rules[0], rules[1], rules[2], rules[3], rules[4], rules[5], rules[6], rules[7] };
    this->_rulesLen = 0;
    for (const auto& val : _rules) {
        _rulesLen += val;
    }
}

using cbool = const bool;

void HeightMap::SetRules(cbool r1, cbool r2, cbool r3, cbool r4, cbool r5, cbool r6, cbool r7, cbool r8) {
    this->_rules = { r1, r2, r3, r4, r5, r6, r7, r8 };
    this->_rulesLen = 0;
    for (const auto& val : _rules) {
        _rulesLen += val;
    }
}

/// <summary>
/// Однопоточный Tick, работает быстро для небольших карт (до ~300*300)
/// </summary>
void HeightMap::Tick(const size_t count) noexcept {
    for (size_t i = 0; i < count; i++) {
        for (size_t x = 0; x < this->Width; x++) {
            for (size_t y = 0; y < this->Height; y++) {
                byte AVG = GetAVGSum(x, y);
                _SecondMatrix->at(x, y) = AVG;
            }
        }
    }
    // Обмениваем указатели вместо копирования данных для лучшей производительности
    Flat2DByte* temp = _MainMatrix;
    _MainMatrix = _SecondMatrix;
    _SecondMatrix = temp;
}

/// <summary>
/// Tick в ThreadsCount потоков, работает быстро при большом размере карты (500*500 и выше)
/// </summary>
void HeightMap::TickAsync(const size_t count) noexcept {
    static const size_t THREADS_COUNT = this->ThreadsCount;
    static const size_t CHUNK_SIZE = (this->Height + THREADS_COUNT - 1) / THREADS_COUNT;

    static vector<size_t> CHUNKS;
    CHUNKS.reserve(THREADS_COUNT * 2);

    if (CHUNKS.empty()) {
        for (size_t i = 0; i < this->Height; i += CHUNK_SIZE) {
            size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

            if (LineFrom + CHUNK_SIZE > _MainMatrix->Width && LineFrom != _MainMatrix->Width) {
                LineTo = _MainMatrix->Width;
            }
            CHUNKS.push_back(LineFrom);
            CHUNKS.push_back(LineTo);
        }
    }

    vector<thread> THREADS;
    THREADS.reserve(THREADS_COUNT);

    for (size_t idx = 0; idx < CHUNKS.size(); idx += 2) {
        // Убедимся, что у нас есть пара (from, to)
        if (idx + 1 < CHUNKS.size()) {
            size_t start = CHUNKS[idx];     // Значение захватывается
            size_t end = CHUNKS[idx + 1];   // Значение захватывается

            THREADS.emplace_back([this, start, end]() { // Захватываем значения start и end по значению
                this->TickAsyncRealization(start, end);
            });
        }
    }

    for (int i = 0; i < count; i++) {
        for (auto& th : THREADS) {
            th.join();
        }
        THREADS.clear();

        // Меняем местами матрицы
        Flat2DByte* temp = _MainMatrix;
        _MainMatrix = _SecondMatrix;
        _SecondMatrix = temp;
    }
}


size_t HeightMap::GetThreadsCount() const {
    return this->ThreadsCount;
}

size_t HeightMap::GetWidth() {
    return _MainMatrix->Width;
}

size_t HeightMap::GetHeight() {
    return _MainMatrix->Height;
}

Flat2DByte* HeightMap::GetMatrix() {
    return _MainMatrix;
}


std::ostream& HeightMap::operator<<(std::ostream& stream)
{
    for (size_t i = 0; i < _MainMatrix->capacity();)
    {
        stream << (int)_MainMatrix->at(i) << "\t";
        i++;
        if (i % Width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}

void HeightMap::SetMatrix(Flat2DByte* matrix)
{
    this->_MainMatrix = matrix;
}

std::ostream& operator<<(std::ostream& stream, HeightMap& map)
{
    for (size_t i = 0; i < map._MainMatrix->capacity();)
    {
        stream << (int)map._MainMatrix->at(i) << "\t";
        i++;
        if (i % map.Width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}


HeightMap::~HeightMap() {
    delete _MainMatrix;
    delete _SecondMatrix;
}