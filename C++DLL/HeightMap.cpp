#include "HeightMap.h"

byte HeightMap::GetAVGSum(size_t x, size_t y) noexcept {

    size_t res = 0;

    // Определяем направления соседей в порядке, соответствующем _rules
    // [0] [1] [2]    (-1,-1) (0,-1) (1,-1)
    // [3]  X  [4]    (-1, 0)        (1, 0)
    // [5] [6] [7]    (-1, 1) (0, 1) (1, 1)

    const int dx[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };
    const int dy[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };

    byte count = 0;
    for (int i = 0; i < 8; i++) {
        // Проверяем, нужно ли учитывать этого соседа согласно правилам
        if (!_rules[i]) {
            continue;
        }

        size_t nx = x + dx[i];
        size_t ny = y + dy[i];

        // Проверяем границы массива
        if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
            byte value = _mainMatrix->at(nx, ny);
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

    size_t Count = this->_width * this->_height;



    for (size_t x = 0; x < Count; x++) {
        _mainMatrix->at(x) = (byte)dis(gen);
    }
}

void HeightMap::TickMTRealization(const size_t lineFrom, const size_t lineTo) {
    size_t iterator = lineFrom;
    for (; iterator < lineTo; iterator++) {
        for (size_t y = 0; y < this->_width; y++) {
            byte AVG = GetAVGSum(y, iterator);
            _secondMatrix->at(y, iterator) = (byte)(AVG * _koef);
        }
    }
}

HeightMap::HeightMap(size_t width, size_t height, bool setRandomValue)
{
    this->_width = width;
    this->_height = height;
    if (setRandomValue) {
        byte* Array = RandomByteArray(this->_width * this->_height, 0, 255);
        this->_mainMatrix = new Flat2DByte(Array, _width, _height);
    }
    else {
        this->_mainMatrix = new Flat2DByte(_width, _height);
    }

    this->_secondMatrix = new Flat2DByte(_width, _height);
}

HeightMap::HeightMap(size_t width, size_t height, int threadsCount, bool setRandomValue)
{
    this->_width = width;
    this->_height = height;
    
    if (setRandomValue) {
        byte* Array = RandomByteArray(this->_width * this->_height, 0, 255);
        this->_mainMatrix = new Flat2DByte(Array, _width, _height);
    } 
    else {
        this->_mainMatrix = new Flat2DByte(_width, _height);
    }

    this->_secondMatrix = new Flat2DByte(_width, _height);
    this->_threadsCount = ::GetThreadsCount(threadsCount);
}

// Конструктор копирования
HeightMap::HeightMap(const HeightMap& other) {
    this->_width = other._width;
    this->_height = other._height;
    this->_rules = other._rules;
    this->_rulesLen = other._rulesLen;

    this->_threadsCount = other._threadsCount;

    this->_mainMatrix = new Flat2DByte(*other._mainMatrix);
    this->_secondMatrix = new Flat2DByte(*other._secondMatrix);
}

// Оператор присваивания
HeightMap& HeightMap::operator=(const HeightMap& other) {
    if (this != &other) {
        delete _mainMatrix;
        delete _secondMatrix;

        this->_width = other._width;
        this->_height = other._height;
        this->_rules = other._rules;
        this->_rulesLen = other._rulesLen;

        this->_threadsCount = other._threadsCount;

        this->_mainMatrix = new Flat2DByte(*other._mainMatrix);
        this->_secondMatrix = new Flat2DByte(*other._secondMatrix);
    }
    return *this;
}

void HeightMap::MakeGood(int type) {
    static double koefBackup = _koef;
    static int constexpr count = 3;
    if (type >= count) type = 0;
    switch (type) {
        case 0:
            this->TickMT(2);
            this->Normalize();
            this->TickMT(1);
            break;
        case 1:
            this->_koef = 2.5f;
            this->TickMT(4);
            this->_koef = 0.6f;
            this->TickMT();
            this->Normalize();
            this->_koef = koefBackup;
            break;
        case 2:
            this->TickMT(2);
            this->Normalize();
            this->_koef = 1.5f;
            this->TickMT(1);
            this->Normalize();
            this->TickMT(1);
            this->Normalize();
            this->_koef = koefBackup;
            break;
        default:
            break;
    }

}

void HeightMap::SetKoef(const double koef) {
    this->_koef = koef;
}

void HeightMap::Normalize() {
    const byte* start = this->_mainMatrix->Array;
    const size_t capacity = this->_mainMatrix->Capacity;
    std::pair<const byte*, const byte*> minmaxPair = std::minmax_element(start, start + capacity);
    
    const byte min = *minmaxPair.first;
    const byte max = *minmaxPair.second;

    for (size_t i = 0; i < capacity; i++) {
        byte at = this->_mainMatrix->at(i);
        int top = at - min;
        int bot = max - min;
        int res = (top * 255 / bot);
        this->_mainMatrix->at(i) = byte(res);
    }
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
        for (size_t x = 0; x < this->_width; x++) {
            for (size_t y = 0; y < this->_height; y++) {
                byte AVG = GetAVGSum(x, y);
                _secondMatrix->at(x, y) = (byte)(AVG * this->_koef);
            }
        }
    }
    // Обмениваем указатели вместо копирования данных для лучшей производительности
    Flat2DByte* temp = _mainMatrix;
    _mainMatrix = _secondMatrix;
    _secondMatrix = temp;
}

/// <summary>
/// Tick в ThreadsCount потоков, работает быстро при большом размере карты (500*500 и выше)
/// </summary>
void HeightMap::TickMT(const size_t count) noexcept {
    static const size_t THREADS_COUNT = this->_threadsCount;
    static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

    static vector<size_t> CHUNKS;
    CHUNKS.reserve(THREADS_COUNT * 2);

    if (CHUNKS.empty()) {
        for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
            size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

            if (LineFrom + CHUNK_SIZE > _mainMatrix->Width && LineFrom != _mainMatrix->Width) {
                LineTo = _mainMatrix->Width;
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
                this->TickMTRealization(start, end);
            });
        }
    }

    for (int i = 0; i < count; i++) {
        for (auto& th : THREADS) {
            th.join();
        }
        THREADS.clear();
    }

    // Меняем местами матрицы
    Flat2DByte* temp = _mainMatrix;
    _mainMatrix = _secondMatrix;
    _secondMatrix = temp;
}


size_t HeightMap::GetThreadsCount() const {
    return this->_threadsCount;
}

size_t HeightMap::GetWidth() {
    return _mainMatrix->Width;
}

size_t HeightMap::GetHeight() {
    return _mainMatrix->Height;
}

Flat2DByte* HeightMap::GetMatrix() {
    return _mainMatrix;
}


std::ostream& HeightMap::operator<<(std::ostream& stream)
{
    for (size_t i = 0; i < _mainMatrix->capacity();)
    {
        stream << (int)_mainMatrix->at(i) << "\t";
        i++;
        if (i % _width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}

void HeightMap::SetMatrix(Flat2DByte* matrix)
{
    this->_mainMatrix = matrix;
}

std::ostream& operator<<(std::ostream& stream, HeightMap& map)
{
    for (size_t i = 0; i < map._mainMatrix->capacity();)
    {
        stream << (int)map._mainMatrix->at(i) << "\t";
        i++;
        if (i % map._width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}


HeightMap::~HeightMap() {
    delete _mainMatrix;
    delete _secondMatrix;
}