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

class Flat2DFloat {
private:
    size_t Capacity;
    size_t Width, Height;
    float* Array;
    friend class HeightMap;
    friend class CaveGenerator;
public:
    Flat2DFloat();

    Flat2DFloat(size_t width, size_t height) noexcept;
    
    Flat2DFloat(float* array, size_t width, size_t height) noexcept;

    Flat2DFloat(const Flat2DFloat& other);

    Flat2DFloat& operator=(const Flat2DFloat& other);

    float& at(size_t x);

    float& at(size_t x, size_t y);

    float& operator[] (size_t posX) noexcept;

    float* data() const noexcept;
    size_t capacity() const;
    size_t width() const;
    size_t height() const;

    friend ostream& operator <<(ostream& stream, Flat2DFloat& data);

    ~Flat2DFloat() noexcept;
};