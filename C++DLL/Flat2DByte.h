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

using byte = uint8_t;

class Flat2DByte {
private:
    size_t Capacity;
    size_t Width, Height;
    byte* Array;
    friend class HeightMap;
public:
    Flat2DByte();

    Flat2DByte(size_t Width, size_t Height) noexcept;

    Flat2DByte(const Flat2DByte& other);

    Flat2DByte& operator=(const Flat2DByte& other);

    byte& at(size_t x);

    byte& at(size_t x, size_t y);

    byte& operator[] (size_t posX) noexcept;

    byte* data() const noexcept;
    size_t capacity() const;
    size_t width() const;
    size_t height() const;

    friend ostream& operator <<(ostream& stream, Flat2DByte& data);

    ~Flat2DByte() noexcept;
};