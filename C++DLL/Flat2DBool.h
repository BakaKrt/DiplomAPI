#pragma once

#include <iostream>
#include <cstddef>
#include <random>
#include <thread>

#ifdef _DEBUG
#include <stdexcept> // Äëÿ std::out_of_range

#endif


using std::vector;
using std::array;
using std::thread;
using std::ostream;

class Flat2DBool {
private:
    size_t Capacity;
    size_t Width, Height;
    bool* Array;
    friend class HeightMap;
    friend class CaveGenerator;
public:
    Flat2DBool();

    Flat2DBool(size_t width, size_t height) noexcept;

    Flat2DBool(bool* array, size_t width, size_t height) noexcept;

    Flat2DBool(const Flat2DBool& other);

    Flat2DBool& operator=(const Flat2DBool& other);

    bool& at(size_t x);

    bool& at(size_t x, size_t y);

    bool& operator[] (size_t posX) noexcept;

    bool* data() const noexcept;
    size_t capacity() const;
    size_t width() const;
    size_t height() const;

    friend ostream& operator <<(ostream& stream, Flat2DBool& data);

    ~Flat2DBool() noexcept;
};