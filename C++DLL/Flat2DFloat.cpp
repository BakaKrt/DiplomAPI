#include "Flat2DFloat.h"



Flat2DFloat::Flat2DFloat() {
    Capacity = 0;
    Width = 0, Height = 0;
    Array = nullptr;
}

Flat2DFloat::Flat2DFloat(size_t width, size_t height) noexcept {
    this->Width = width;
    this->Height = height;
    this->Capacity = this->Width * this->Height;
    this->Array = new float[Capacity]();
}

Flat2DFloat::Flat2DFloat(float* array, size_t width, size_t height) noexcept
{
    this->Array = array;
    this->Capacity = width * height;
    this->Width = width;
    this->Height = height;
}

Flat2DFloat::Flat2DFloat(const Flat2DFloat& other) {
    Width = other.Width;
    Height = other.Height;
    Capacity = other.Capacity;

    if (other.Array) {
        Array = new float[Capacity];
        std::memcpy(Array, other.Array, Capacity);
    }
    else {
        Array = nullptr;
    }
}

Flat2DFloat& Flat2DFloat::operator=(const Flat2DFloat& other) {
    if (this != &other) { // Проверка на самоприсваивание
        delete[] Array; // Освобождаем старую память

        Width = other.Width;
        Height = other.Height;
        Capacity = other.Capacity;

        if (other.Array) {
            Array = new float[Capacity];
            std::memcpy(Array, other.Array, Capacity);
        }
        else {
            Array = nullptr;
        }
    }
    return *this;
}

float& Flat2DFloat::at(size_t x) {
    //#ifdef _DEBUG
    //    if (x >= Capacity) {
    //        throw std::out_of_range("Index out of range");
    //    }
    //#endif // DEBUG
    return Array[x];
}

float& Flat2DFloat::at(size_t x, size_t y) {
    //#ifdef _DEBUG
    //    if (x >= Width || y >= Height) {
    //        throw std::out_of_range("Index out of range");
    //    }
    //#endif // DEBUG
    return Array[y * Width + x];
}

float& Flat2DFloat::operator[] (size_t posX) noexcept {
    return Array[posX];
}

float* Flat2DFloat::data() const noexcept {
    return Array;
}
size_t Flat2DFloat::capacity() const { return this->Capacity; }
size_t Flat2DFloat::width()    const { return this->Width; }
size_t Flat2DFloat::height()   const { return this->Height; }

std::ostream& operator <<(std::ostream& stream, Flat2DFloat& data) {
    for (size_t i = 0; i < data.Capacity;)
    {
        stream << (int)data.at(i) << "\t";
        i++;
        if (i % data.Width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}

Flat2DFloat::~Flat2DFloat() noexcept {
    delete[] Array;
}